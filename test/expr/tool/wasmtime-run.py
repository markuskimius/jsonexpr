#!/bin/bash

##############################################################################
# BOOTSTRAP
#
# Include ../lib in the search path then call python3 or python.
# (Thanks to https://unix.stackexchange.com/questions/20880)
#
if "true" : '''\'
then
    export PYTHONPATH="$(dirname $0)/../lib:$PYTHONPATH"
    pythoncmd=python

    if command -v python3 >/dev/null; then
        pythoncmd=python3
    fi

    exec "$pythoncmd" "$0" "$@"
    exit 127
fi
'''

##############################################################################
# PYTHON CODE BEGINS HERE

import os
import sys
import errno
import getopts
from wasmtime import Store, Module, FuncType, ValType, Linker, Engine

__copyright__ = "Copyright 2024-2025 Mark Kim"
__license__ = "Apache 2.0"
__author__ = "Mark Kim"


##############################################################################
# GLOBALS

SCRIPTNAME = os.path.basename(__file__)
SCRIPTDIR = os.path.dirname(__file__)

class opts:
    wasmfile = os.path.join(SCRIPTDIR, "..", "lib", "je.wasm")
    quiet = 0
    eval = []
    files = []


##############################################################################
# USAGE

def usage():
    """\
Run a JSONexpr script.

Usage: {SCRIPTNAME} [OPTIONS] [FILES]

Options:
  FILE                  File(s) to run.

  -e,--eval EXPR        Evaluate EXPR.
  -q,--quiet            Do not output the result.
"""

    print(usage.__doc__.format(**globals()))


##############################################################################
# MAIN

def main():
    errcount = 0

    # Process options
    getopt = getopts.getopts(sys.argv, {
        "e" : 1, "eval"    : 1,
        "q" : 0, "quiet"   : 0,
        "h" : 0, "help"    : 0,
    })

    for c in getopt:
        if   c in ("-")            : opts.files.append(getopt.optarg)
        elif c in ("e", "eval")    : opts.eval.append(getopt.optarg)
        elif c in ("q", "quiet")   : opts.quiet = 1
        elif c in ("h", "help")    : usage(); sys.exit(0)
        else                       : errcount += 1

    # Sanity check
    if errcount:
        sys.stderr.write("Type '{SCRIPTNAME} --help' for help.\n".format(**globals()))
        sys.exit(1)

    for expr in opts.eval:
        doMyThing(expr)

    if len(opts.files) == 0 and len(opts.eval) == 0:
        doMyFdThing(sys.stdin)

    for f in opts.files:
        with open(f, mode="rt") as exprfd:
            doMyFdThing(exprfd)

class Exit(Exception):
    def __init__(self, code):
        self.code = code

class Iface:
    def __init__(self):
        self.engine = Engine()
        self.store = Store(self.engine)
        self.linker = Linker(self.engine)

        self.linker.define_func("env", "read" , FuncType([ValType.i32(), ValType.i32(), ValType.i32()], [ValType.i32()]), self.read)
        self.linker.define_func("env", "write", FuncType([ValType.i32(), ValType.i32(), ValType.i32()], [ValType.i32()]), self.write)
        self.linker.define_func("env", "_exit", FuncType([ValType.i32()], []), self._exit)

        self.module = Module.from_file(self.engine, opts.wasmfile)
        self.instance = self.linker.instantiate(self.store, self.module)
        self.memory8 = self.instance.exports(self.store)["memory"].get_buffer_ptr(self.store)
        self._start = self.instance.exports(self.store)["_start"]

    def read(self, fd:"i32", buf:"i32", count:"i32") -> "i32":
        data = os.read(fd, count)
        count = len(data)

        self.memory8 = self.instance.exports(self.store)["memory"].get_buffer_ptr(self.store)
        self.memory8[buf:buf+count] = data

        return count

    def write(self, fd:"i32", buf:"i32", count:"i32") -> "i32":
        self.memory8 = self.instance.exports(self.store)["memory"].get_buffer_ptr(self.store)

        return os.write(fd, bytearray(self.memory8[buf:buf+count]))

    def _exit(self, status:"i32") -> None:
        raise Exit(status)

    def strdup(self, string):
        encoded = string.encode("utf-8")
        waddr = self.instance.exports(self.store)["calloc"](self.store, 1, len(encoded)+1)
        self.memory8 = self.instance.exports(self.store)["memory"].get_buffer_ptr(self.store)
        self.memory8[waddr:waddr+len(string)] = encoded
        self.memory8[waddr+len(encoded)] = 0

        return waddr

    def free(self, waddr):
        self.instance.exports(self.store)["free"](self.store, waddr)

    def parse(self, wcode):
        return self.instance.exports(self.store)["parse"](self.store, wcode)

    def newsym(self, wparent=0):
        return self.instance.exports(self.store)["newsym"](self.store, wparent)

    def asteval(self, wast, wsymmap):
        return self.instance.exports(self.store)["asteval"](self.store, wast, wsymmap)

    def valfree(self, wval):
        self.instance.exports(self.store)["valfree"](self.store, wval)

    def symfree(self, wsymmap):
        return self.instance.exports(self.store)["symfree"](self.store, wsymmap)

    def astfree(self, wast):
        self.instance.exports(self.store)["astfree"](self.store, wast)

def doMyFdThing(exprfd):
    expr = exprfd.read()

    return doMyThing(expr)

def doMyThing(expr):
    iface = Iface()
    exitcode = 0

    try:
        code = iface.strdup(expr)
        ast = iface.parse(code)
        symmap = iface.newsym(0)
        value = iface.asteval(ast, symmap)

        iface.valfree(value)
        iface.symfree(symmap)
        iface.astfree(ast)
        iface.free(code)
    except Exit as e:
        exitcode = e.code

    return exitcode


##############################################################################
# ENTRY POINT

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("")
        sys.exit(errno.EOWNERDEAD)


# vim:ft=python
