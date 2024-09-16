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
import asyncio
import tornado.web

__copyright__ = "Copyright 2022-2024 Mark Kim"
__license__ = "Apache 2.0"
__author__ = "Mark Kim"


##############################################################################
# GLOBALS

SCRIPTNAME = os.path.basename(__file__)
SCRIPTDIR = os.path.dirname(__file__)

class opts:
    files = []
    port = 8080
    server = "localhost"
    libdir = os.path.join(SCRIPTDIR, "lib")


##############################################################################
# USAGE

def usage():
    """\
A skeleton program.

Usage: {SCRIPTNAME} [OPTIONS]

Options:
  -s,--server=SERVER    Listen on SERVER [default={opts.server}].
  -p,--port=PORT        Listen on PORT [default={opts.port}].
"""

    print(usage.__doc__.format(**globals()))


##############################################################################
# MAIN

def main():
    errcount = 0

    # Process options
    getopt = getopts.getopts(sys.argv, {
        "s" : 1     , "server" : 1     ,
        "p" : isInt , "port"   : isInt ,
        "h" : 0     , "help"   : 0,
    })

    for c in getopt:
        if c in ("-")             : opts.files.append(getopt.optarg)
        elif c in ("s", "server") : opts.server = getopt.optarg
        elif c in ("p", "port")   : opts.port = int(getopt.optarg)
        elif c in ("h", "help")   : usage(); sys.exit(0)
        else                      : errcount += 1

    # Sanity check
    if errcount:
        sys.stderr.write("Type `{SCRIPTNAME} --help` for help.\n".format(**globals()))
        sys.exit(1)

    doMyThing()


def isInt(value):
    is_int = True

    try:
        int(value)
    except ValueError:
        is_int = False

    return is_int


def doMyThing():
    loop = asyncio.get_event_loop()
    app = tornado.web.Application([
        (r"/", Index),
        (r"/lib/(.*)", tornado.web.StaticFileHandler, {"path": opts.libdir}),
    ])

    app.listen(opts.port, opts.server)
    loop.run_forever()


##############################################################################
# Routes

class Index(tornado.web.RequestHandler):
    def get(self):
        self.render("index.html")


##############################################################################
# ENTRY POINT

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("")
        sys.exit(errno.EOWNERDEAD)


# vim:ft=python
