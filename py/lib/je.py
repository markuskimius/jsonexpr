__copyright__ = "Copyright 2024 Mark Kim"
__license__ = "Apache 2.0"
__version__ = "0.1.0"
__author__ = "Mark Kim"

import re
import os
import json
import wasmer
import wasmer_compiler_cranelift


##############################################################################
# PUBLIC FUNCTIONS

def evalfd(fd, symbols=None):
    code = fd.read()

    return eval(code, symbols)

def eval(code, symbols=None):
    compiled = compile(str(code)) 

    if symbols is not None:
        compiled.setSymbols(symbols)

    return compiled.eval()

def compilefd(fd):
    code = fd.read()

    return compile(code)

def compile(code):
    return Compiled(code)


##############################################################################
# HELPER CLASSES

class Compiled:
    def __init__(self, code):
        scriptdir = os.path.dirname(__file__)
        wasmfile = os.path.join(scriptdir, "je.wasm")

        # Setup
        with open(wasmfile, mode="rb") as fd:
            self.store = wasmer.Store()
            self.module = wasmer.Module(self.store, fd.read())

        self.iface = Interface()
        self.instance = wasmer.Instance(self.module, {
            "env": {
                "read"   : wasmer.Function(self.store, self.iface.read),
                "write"  : wasmer.Function(self.store, self.iface.write),
                "_exit"  : wasmer.Function(self.store, self.iface._exit),
            }
        })
        self.iface.memory8 = self.instance.exports.memory.uint8_view()
        self.symtbl = self.instance.exports.je_newtable(0)
        self.util = Util(self.instance)

        # Parse
        self.code = self.util.strdup(code)
        self.tree = self.instance.exports.je_parse(self.code)

    def eval(self):
        vresult = self.instance.exports.je_eval(self.tree, self.symtbl)
        cquoted = self.instance.exports.je_valqstr(vresult)
        quoted = self.util.strat(cquoted)
        result = json_loads(quoted, self.instance, vresult)

        self.instance.exports.je_freeval(vresult)

        return result

    def __contains__(self, name):
        cname = self.util.strdup(name)
        result = self.instance.exports.je_tableget(self.symtbl, cname)

        self.instance.exports.free(cname)

        return result != 0
    
    def __getitem__(self, name):
        jstr = self.getJson(name)
        cname = self.util.strdup(name)
        symval = self.instance.exports.je_tableget(self.symtbl, cname)

        self.instance.exports.free(cname)

        return json_loads(jstr, self.instance, symval)
    
    def __setitem__(self, name, value):
        self.setJson(name, json_dumps(value))

    def __delitem__(self, name):
        cname = self.util.strdup(name)

        self.instance.exports.je_tableunset(self.symtbl, cname)
        self.instance.exports.free(cname)

    def setSymbols(self, symbols):
        for key,value in symbols.items():
            self.setJson(key, json_dumps(value))

    def setJson(self, name, jstr):
        # Verify name is an identifier
        if not re.match(r"^[_a-zA-Z][_a-zA-Z0-9]*$", name, re.MULTILINE):
            raise SyntaxError(f"Identifier expected, got `{name}`")

        expr = f"{name} = {jstr};"
        cexpr = self.util.strdup(expr)
        tree = self.instance.exports.je_parse(cexpr)
        result = self.instance.exports.je_eval(tree, self.symtbl)

        self.instance.exports.je_freenode(tree)
        self.instance.exports.je_freeval(result)
        self.instance.exports.free(cexpr)

    def getJson(self, name):
        cname = self.util.strdup(name)
        result = self.instance.exports.je_tableget(self.symtbl, cname)
        cstr = self.instance.exports.je_valqstr(result)
        jstr = self.util.strat(cstr)

        self.instance.exports.free(cname)

        return jstr

class Interface:
    def read(self, fd:"i32", buf:"i32", count:"i32") -> "i32":
        data = os.read(fd, count)
        count = len(data)
        self.memory8[buf:buf+count] = data

        return count

    def write(self, fd:"i32", buf:"i32", count:"i32") -> "i32":
        return os.write(fd, bytearray(self.memory8[buf:buf+count]))

    def _exit(self, status:"i32") -> None:
        raise Exit(status)

class Util:
    def __init__(self, instance):
        self.memory8 = instance.exports.memory.uint8_view()
        self.instance = instance
        self.allocated = []

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        for addr in self.allocated:
            self.instance.exports.free(addr)

        self.allocated = []

    def strat(self, addr):
        len = 0

        while(self.memory8[addr+len] != 0):
            len += 1

        return bytearray(self.memory8[addr:addr+len]).decode("utf-8")

    def strdup(self, string):
        encoded = string.encode("utf-8")
        addr = self.instance.exports.calloc(1, len(encoded)+1)
        self.memory8[addr:addr+len(string)] = encoded
        self.memory8[addr+len(encoded)] = 0

        self.allocated += [addr]

        return addr

def _cvalue(instance, pyvalue):
    if   isinstance(pyvalue, bool):
        cvalue = instance.exports.je_boolval(pyvalue)
    elif isinstance(pyvalue, int):
        cvalue = instance.exports.je_intval(pyvalue)
    elif isinstance(pyvalue, float):
        cvalue = instance.exports.je_dblval(pyvalue)
    elif isinstance(pyvalue, list):
        cvalue = instance.exports.je_arrval(0)
        carray = instance.exports.je_getarray(cvalue)

        for i in range(len(pyvalue)):
            item = pyvalue[i]
            (cval, pval) = _cvalue(instance, item)

            instance.exports.je_vecpush(carray, cval)
            pyvalue[i] = pval

        pyvalue = Array(pyvalue, instance, carray)

    elif isinstance(pyvalue, dict):
        cvalue = instance.exports.je_objval(0)
        cobject = instance.exports.je_getobject(cvalue)

        with Util(instance) as util:
            for k,v in pyvalue.items():
                ck = util.strdup(str(k))
                (cv, pv) = _cvalue(instance, v)

                instance.exports.je_mapset(cobject, ck, cv)
                pyvalue[k] = pv

        pyvalue = Object(pyvalue, instance, cobject)

    else:
        with Util(instance) as util:
            pyvalue = str(pyvalue)
            cstr = util.strdup(pyvalue)
            cvalue = instance.exports.je_strval(cstr)

    return (cvalue, pyvalue)

class Object(dict):
    def __init__(self, obj, instance, symmap):
        super().__init__(obj)
        self.__instance = instance
        self.__symmap = symmap

    def __setitem__(self, name, value):
        with Util(self.__instance) as util:
            cname = util.strdup(str(name))
            (cvalue, pvalue) = _cvalue(self.__instance, value)

            self.__instance.exports.je_mapset(self.__symmap, cname, cvalue)

        return super().__setitem__(name, pvalue)

    def __delitem__(self, name):
        with Util(self.__instance) as util:
            cname = util.strdup(name)

            self.__instance.exports.je_mapunset(self.__symmap, cname)

        return super().__delitem__(name)

class Array(list):
    def __init__(self, array, instance, symvec):
        super().__init__(array)
        self.__instance = instance
        self.__symvec = symvec

    def __setitem__(self, index, value):
        (cvalue, pvalue) = _cvalue(self.__instance, value)

        self.__instance.exports.je_vecset(self.__symvec, index, cvalue)

        return super().__setitem__(index, pvalue)

    def __delitem__(self, index):
        self.__instance.exports.je_vecunset(self.__symvec, index)

        return super().__delitem__(index)


##############################################################################
# HELPER FUNCTIONS

def json_dumps(value):
    return json.dumps(value, default=str)

def json_loads(jstr, instance, symval):
    result = json.loads(jstr)

    return _enrich(result, instance, symval);

def _enrich(value, instance, symval):
    if   isinstance(value, dict):
        symmap = instance.exports.je_getobject(symval)

        for k,v in value.items():
            with Util(instance) as util:
                cname = util.strdup(k)
                csymval = instance.exports.je_mapget(symmap, cname)
                value[k] = _enrich(v, instance, csymval)

        value = Object(value, instance, symmap)

    elif isinstance(value, list):
        symvec = instance.exports.je_getarray(symval)

        for i in range(len(value)):
            csymval = instance.exports.je_vecget(symvec, i)
            value[i] = _enrich(value[i], instance, csymval)

        value = Array(value, instance, symvec)

    return value


##############################################################################
# EXCEPTIONS

class JeException(Exception):
    pass

class SyntaxError(JeException):
    def __init__(self, text):
        self.text = text

class Exit(JeException):
    def __init__(self, code):
        self.code = code


##############################################################################
# TEST CODE

if __name__ == "__main__":
    import sys
    import errno

    def main():
        compiled = compile("""
            PRINT("I have " + LEN(grades) + " students");
            PRINT("Alice's grade is " + grades.alice);
        """)

        compiled.setSymbols({
            "grades" : {
                "alice" : "A",
                "bob"   : "B",
            },
            "nested" : {
                "alice" : {
                    "grade"     : [ "A", "B", "C" ],
                    "last_name" : "Smith",
                },
                "bob"   : {
                    "grade"     : "B",
                    "last_name" : "Johnson",
                },
            },
        });

        result = compiled.eval()

        # compiled["nested"]["alice"]["grade"] = [ 1, 3, 2 ]
        # compiled["nested"]["alice"]["grade"][1] = {
        #     "a" : True,
        #     "b" : True,
        #     "c" : False,
        # }
        # compiled["nested"]["alice"]["grade"][1]["b"] = "Hello!"
        # compiled["nested"]["alice"]["grade"] = {
        #     "a" : "b",
        #     "c" : "d",
        # }
        # print("hello" in compiled)
        # print("grades" in compiled)
        print(json.dumps(compiled["nested"], indent=2))

        # print(type(result), result)

    try:
        main()
    except Exit as e:
        sys.exit(e.code)
    except KeyboardInterrupt:
        print("")
        sys.exit(errno.EOWNERDEAD)

