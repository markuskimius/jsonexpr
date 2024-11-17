#!/bin/bash

for file in *.je; do
    printf "%-15s .. " "$file"

    if diff -q <(../bin/je -q "$file" 2>&1) "out/$(basename "$file" .je).out" >&/dev/null; then
        printf "[OK]\n"
    else
        printf "[ERROR]\n"
    fi
done
