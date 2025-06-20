#!/bin/bash

##############################################################################
# jsonexpr test code
# https://github.com/markuskimius/jsonexpr
#
# Copyright (c)2024-2025 Mark K. Kim
# Released under the Apache license 2.0
# https://github.com/markuskimius/jsonexpr/blob/master/LICENSE
##############################################################################

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

import sys
import json
import errno
import jsonexpr as je

def main():
    instance = je.instance()
    parsed = instance.parse("""
        PRINT("I have " + LEN(grades) + " students");
        PRINT("Alice's grade is " + grades.alice);
        grades
    """)
    symmap = instance.symmap({
        "grades" : {
            "alice"   : "A",
            "bob"     : "B",
            "charlie" : "B",
        }
    })
    result = parsed.eval(symmap)
    print()

    print("= 1 =")
    print("symmap :", symmap["grades"], len(symmap["grades"]))
    print("result :", result, len(result))
    print()

    print("= 2 =")
    symmap["grades"]["bob"] = [ "A", "B", "C" ]
    result["bob"][1] = "D"
    print("symmap :", symmap["grades"], len(symmap["grades"]))
    print("result :", result, len(result))
    print()

    print("= 3 =")
    del symmap["grades"]["alice"]
    print("symmap :", symmap["grades"], len(symmap["grades"]))
    print("result :", result, len(result))
    print()

    print("= 4 =")
    symmap["grades"] = [ "A", "B", "C" ]
    result["bob"] = [ "D", "F" ]
    print("symmap :", symmap["grades"], len(symmap["grades"]))
    print("result :", result, len(result))
    print()

    print("= 5 =")
    symmap["grades"] = { "alice":"A", "bob":"B", "charlie":"C" }
    symmap["grades"]["charlie"] = result["bob"]
    result["bob"][0] = "E"
    print("symmap :", symmap["grades"], len(symmap["grades"]))
    print("result :", result, len(result))
    print()

    print("= 6 =")
    symmap["grades"] = { "alice":"A", "bob":result["bob"], "charlie":result["bob"] }
    print("symmap :", symmap["grades"], len(symmap["grades"]))
    print("result :", result, len(result))
    print()

    print("= 7 =")
    result["bob"][0] = "F"
    print("symmap :", symmap["grades"], len(symmap["grades"]))
    print("result :", result, len(result))
    print()

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("")
        sys.exit(errno.EOWNERDEAD)

