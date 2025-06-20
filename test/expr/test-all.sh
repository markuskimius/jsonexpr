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
  -a,--arch             Test the binary version only.
  -p,--python           Test the python version only.
  -w,--wasm             Test the wasm version only.
                        By default, all versions are tested.

  FILE                  File(s) to test (Default=*.je)

EOF
}


##############################################################################
# PROGRAM BEGINS HERE

source "getopt.sh" || exit 1

SCRIPTNAME=$(basename "${BASH_SOURCE}")
BINARY=../../build/$(uname -m)/bin/je
PYTHON=$(command -v je)
SCRIPT=tool/wasmer-run.py
WASMFILE=lib/je.wasm
OUT=var/output/%s-%s.out
DIFF=var/diff/%s-%s.diff
PERF=var/perf/%s_PERF_$(date +%Y%m%d_%H%M%S).log
REFOUT=ref/%s.out
REFDIFF=ref/%s.diff
FILES=()
MODE=()

function main() {
    local OPTOPT OPTARG
    local exitcode=0
    local mode

    # Process options
    while getopt-sh "hapw" "help,arch,python,wasm" "$@"; do
        case "$OPTOPT" in
            -a|--arch)   MODE+=( ARCH ) ;;
            -p|--python) MODE+=( PYTHON ) ;;
            -w|--wasm)   MODE+=( WASM ) ;;
            -h|--help)   usage && exit 0 ;;
            *)           exitcode=1      ;;
        esac
    done

    # Default mode
    if (( ${#MODE[@]} == 0 )); then
        MODE=( ARCH PYTHON WASM )
    fi

    # Default files
    if (( ${#OPTARG[@]} )); then
        FILES=( "${OPTARG[@]}" )
    else
        FILES=( *.je )
    fi

    # Validate
    if [[ ! -r "$WASMFILE" ]]; then
        printf '%s required; run `make -C .. install` first\n' "$WASMFILE"
        exitcode=1
    fi

    # Sanity check
    if (( exitcode )); then
        echo && usage
        exit "$exitcode"
    fi 1>&2

    for mode in "${MODE[@]}"; do
        doMyThing "$mode" "${FILES[@]}"
    done
}

function doMyThing() {
    local mode=$1; shift
    local out=$(printf "$OUT" "$mode" "$1")
    local diff=$(printf "$DIFF" "$mode" "$1")
    local perf=$(printf "$PERF" "$mode")
    local refout=$(printf "$REFOUT" "$1")
    local refdiff=$(printf "$REFDIFF" "$1")
    local prevtime
    local nexttime
    local testbin

    # Handle mode
    case "$mode" in
        ARCH)   testbin=$BINARY ;;
        PYTHON) testbin=$PYTHON ;;
        WASM)   testbin=$SCRIPT ;;
    esac

    if true; then
        # Create all required directories
        mkdir -p "${out%/*}"
        mkdir -p "${diff%/*}"
        mkdir -p "${perf%/*}"
        mkdir -p "${refout%/*}"
        mkdir -p "${refdiff%/*}"

        # Test information
        printf "== %s TEST ==\n" "$mode"

        # State information
        printf "%-15s %s  %s\n" "$(basename "$testbin")" "$(md5sum "$testbin" | cut -f1 -d' ')" "$(stat -c %z "$testbin")"
        if [[ "$mode" == "WASM" ]]; then
            printf "%-15s %s  %s\n" "$(basename "$WASMFILE")" "$(md5sum "$WASMFILE" | cut -f1 -d' ')" "$(stat -c %z "$WASMFILE")"
        fi
        for file in "$@"; do
            printf "%-15s %s  %s\n" "$file" "$(md5sum "$file" | cut -f1 -d' ')" "$(stat -c %z "$file")"
        done
        echo

        # Test each file
        for file in "$@"; do
            basename=${file%.*}
            out=$(printf "$OUT" "$mode" "$basename")
            diff=$(printf "$DIFF" "$mode" "$basename")
            refout=$(printf "$REFOUT" "$basename")
            refdiff=$(printf "$REFDIFF" "$basename")

            printf "%-15s .. " "$file"

            prevtime=$(date '+%s.%N')
            "$testbin" -q "$file" &> "$out"
            nexttime=$(date '+%s.%N')

            if diff -u "$refout" "$out" > "$diff"; then
                rm -f "$diff"
                printf "[PASS] "
            elif [[ -r "$refdiff" ]]; then
                if grep -vFf "$refdiff" <(tail -n +3 "$diff") | grep '^[-+]' >&/dev/null; then
                    printf "[ERROR]"
                else
                    printf "[OK]   "
                fi
            else
                printf "[ERROR]"
            fi

            printf " %9.3fms\n" "$("$BINARY" -e "($nexttime - $prevtime) * 1000")"
        done
        echo
    fi 2>&1 | tee "$perf"
}


##############################################################################
# ENTRY POINT

main "$@"
