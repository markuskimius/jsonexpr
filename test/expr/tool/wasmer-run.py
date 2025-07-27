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
import importlib.util
from wasmer import engine, Store, Module, Instance, ImportObject, Function, FunctionType, Type

if importlib.util.find_spec("wasmer_compiler_llvm") is not None:
    from wasmer_compiler_llvm import Compiler
else:
    from wasmer_compiler_cranelift import Compiler

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

    with open(opts.wasmfile, mode="rb") as wasmfd:
        wasm = wasmfd.read()

        for expr in opts.eval:
            doMyThing(wasm, expr)

        if len(opts.files) == 0 and len(opts.eval) == 0:
            doMyFdThing(wasm, sys.stdin)

        for f in opts.files:
            with open(f, mode="rt") as exprfd:
                doMyFdThing(wasm, exprfd)

class Exit(Exception):
    def __init__(self, code):
        self.code = code

class Iface:
    def __init__(self):
        self.allocated = [];

    def read(self, fd:"i32", buf:"i32", count:"i32") -> "i32":
        data = os.read(fd, count)
        count = len(data);
        self.memory8[buf:buf+count] = data;

        return count

    def write(self, fd:"i32", buf:"i32", count:"i32") -> "i32":
        return os.write(fd, bytearray(self.memory8[buf:buf+count]))

    def strdup(self, string) -> int:
        encoded = string.encode("utf-8")
        waddr = self.instance.exports.calloc(1, len(encoded)+1)
        self.memory8[waddr:waddr+len(string)] = encoded
        self.memory8[waddr+len(encoded)] = 0

        self.allocated += [waddr]

        return waddr;

    def free(self, waddr):
        if waddr in self.allocated:
            self.allocated.remove(waddr)

        self.instance.exports.free(waddr)

    def _exit(self, status:"i32") -> None:
        raise Exit(status)

def doMyFdThing(wasm, exprfd):
    expr = exprfd.read()

    return doMyThing(wasm, expr)

def doMyThing(wasm, expr):
    store = Store()
    iface = Iface()
    exitcode = 0

    module = Module(store, wasm)
    instance = Instance(module, {
        "env": {
            "read"   : Function(store, iface.read),
            "write"  : Function(store, iface.write),
            "_exit"  : Function(store, iface._exit),
        }
    })

    iface.instance = instance;
    iface.memory8 = instance.exports.memory.uint8_view()

    try:
        code = iface.strdup(expr)
        ast = instance.exports.parse(code)
        symmap = instance.exports.newsym(0)
        value = instance.exports.asteval(ast, symmap, 0)

        instance.exports.valfree(value)
        instance.exports.symfree(symmap)
        instance.exports.astfree(ast)
        iface.free(code);
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
