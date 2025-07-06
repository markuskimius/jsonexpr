#!/usr/bin/env bash

##############################################################################
# Test script
# https://github.com/markuskimius/jsonexpr
#
# Copyright (c)2024-2025 Mark K. Kim
# Released under the Apache license 2.0
# https://github.com/markuskimius/jsonexpr/blob/master/LICENSE
##############################################################################

function usage() {
    cat <<EOF
JSONexpr test script.

Usage: ${SCRIPTNAME} [OPTIONS] [FILES]

Options:
  FILE                  File(s) to test (Default=*.c)

EOF
}


##############################################################################
# PROGRAM BEGINS HERE

source "getopt.sh" || exit 1

SCRIPTNAME=$(basename "${BASH_SOURCE}")
ARCH=$(uname -m)
ARCHBIN=build/${ARCH}/%s
WASMBIN=build/wasm/%s.wasm
WASMRUNNER=tool/wasmtime-run.py
PYTHONCMD=python
OUT=var/output/%s-%s.out
DIFF=var/diff/%s.diff
REFDIFF=ref/%s.diff
FILES=()

if command -v python3 >/dev/null; then
    PYTHONCMD=python3
fi

if "$PYTHONCMD" -c 'import wasmer' 2>/dev/null; then
    WASMRUNNER=tool/wasmer-run.py
fi

function main() {
    local OPTOPT OPTARG
    local exitcode=0

    # Process options
    while getopt-sh "h" "help" "$@"; do
        case "$OPTOPT" in
            -h|--help)  usage && exit 0 ;;
            *)          exitcode=1      ;;
        esac
    done

    # Default files
    if (( ${#OPTARG[@]} )); then
        FILES=( "${OPTARG[@]}" )
    else
        FILES=( *.c )
    fi

    # Sanity check
    if (( exitcode )); then
        echo && usage
        exit "$exitcode"
    fi 1>&2

    doMyThing "${FILES[@]}"
}

function doMyThing() {
    local archout=$(printf "$OUT" "ARCH" "$1")
    local wasmout=$(printf "$OUT" "WASM" "$1")
    local diff=$(printf "$DIFF" "$1")
    local refdiff=$(printf "$REFDIFF" "$1")

    # Create all required directories
    mkdir -p "${archout%/*}"
    mkdir -p "${wasmout%/*}"
    mkdir -p "${diff%/*}"
    mkdir -p "${refdiff%/*}"

    # Test each file
    for file in "$@"; do
        basename=${file%.*}
        archbin=$(printf "$ARCHBIN" "$basename")
        wasmbin=$(printf "$WASMBIN" "$basename")
        archout=$(printf "$OUT" "ARCH" "$basename")
        wasmout=$(printf "$OUT" "WASM" "$basename")
        diff=$(printf "$DIFF" "$basename")
        refout=$(printf "$REFOUT" "$basename")
        refdiff=$(printf "$REFDIFF" "$basename")

        printf "%-15s .. " "$file"

        "$archbin" > "$archout"
        "$WASMRUNNER" "$wasmbin" > "$wasmout"

        if diff -u "$archout" "$wasmout" > "$diff"; then
            printf "[PASS]\n"
        elif [[ -r "$refdiff" ]]; then
            if grep -vFf "$refdiff" <(tail -n +3 "$diff") | grep '^[-+]' >&/dev/null; then
                printf "[ERROR]\n"
            else
                printf "[OK]\n"
            fi
        else
            printf "[ERROR]\n"
        fi
    done
}


##############################################################################
# ENTRY POINT

main "$@"
