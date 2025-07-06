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

class opts:
    files = []
    nrun = 1


##############################################################################
# USAGE

def usage():
    """\
A program to run a wasm binary.

Usage: {SCRIPTNAME} [OPTIONS] [FILES]

Options:
  FILE                  WASM file(s) to execute.
  -n <N>                Run it N times (Default={opts.nrun})
"""

    print(usage.__doc__.format(**globals()))


##############################################################################
# MAIN

def main():
    errcount = 0

    # Process options
    getopt = getopts.getopts(sys.argv, {
        "h" :      0, "help" : 0,
        "n" : is_int,
    })

    for c in getopt:
        if   c in ("-")         : opts.files.append(getopt.optarg)
        elif c in ("n")         : opts.nrun = int(getopt.optarg)
        elif c in ("h", "help") : usage(); sys.exit(0)
        else                    : errcount += 1

    # Sanity check
    if errcount:
        sys.stderr.write("Type `{SCRIPTNAME} --help` for help.\n".format(**globals()))
        sys.exit(1)

    for file in opts.files:
        doMyThing(file)


def is_int(s_int):
    isint = True

    try: int(s_int)
    except: isint = False

    return isint


class Exit(Exception):
    def __init__(self, code):
        self.code = code


def doMyThing(file):
    last = 0

    def __read(fd, buf, count):
        data = os.read(fd, count)
        count = len(data);
        memory8[buf:buf+count] = data;

        return count

    def __write(fd, buf, count):
        return os.write(fd, bytearray(memory8[buf:buf+count]))

    def __exit(status):
        raise Exit(status)

    engine = Engine()
    store = Store(engine)
    linker = Linker(engine)

    linker.define_func("env",  "read", FuncType([ValType.i32(), ValType.i32(), ValType.i32()], [ValType.i32()]), __read)
    linker.define_func("env", "write", FuncType([ValType.i32(), ValType.i32(), ValType.i32()], [ValType.i32()]), __write)
    linker.define_func("env", "_exit", FuncType([ValType.i32()], []), __exit)

    module = Module.from_file(engine, file)
    instance = linker.instantiate(store, module)

    memory8 = instance.exports(store)["memory"].get_buffer_ptr(store)
    _start = instance.exports(store)["_start"]

    for i in range(opts.nrun):
        try:
            _start(store)
        except Exit as e:
            last = e.code

    return last


##############################################################################
# ENTRY POINT

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("")
        sys.exit(errno.EOWNERDEAD)


# vim:ft=python
