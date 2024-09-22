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
        result = self.instance.exports.je_eval(self.tree, self.symtbl)
        cquoted = self.instance.exports.je_valqstr(result)
        quoted = self.util.strat(cquoted)
        self.instance.exports.je_freeval(result)

        return json.loads(quoted)

    def __contains__(self, name):
        cname = self.util.strdup(name)
        result = self.instance.exports.je_tableget(self.symtbl, cname)

        self.instance.exports.free(cname)

        return result is not None
    
    def __getitem__(self, name):
        jstr = self.getJson(name)

        return json.loads(jstr)
    
    def __setitem__(self, name, value):
        self.setJson(name, json.dumps(value, default=str))
    
        return value

    def __delitem__(self, name):
        cname = self.util.strdup(name)
        self.instance.exports.je_tableunset(self.symtbl, cname)

    def setSymbols(self, symbols):
        for key,value in symbols.items():
            self.setJson(key, json.dumps(value, default=str))

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

    def main():
        compiled = compile("""
            PRINT("I have " + LEN(grades) + " students");
            PRINT("Alice's grade is " + grades.alice);
        """)

        compiled.setSymbols({
            "grades" : {
                "alice" : "A",
                "bob"   : "B",
            }
        });

        result = compiled.eval()
        print(json.dumps(compiled["grades"], indent=2))

        # print(type(result), result)

    try:
        main()
    except Exit as e:
        sys.exit(e.code)
    except KeyboardInterrupt:
        print("")
        sys.exit(errno.EOWNERDEAD)

