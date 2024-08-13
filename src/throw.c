#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "node.h"
#include "throw.h"
#include "util.h"


/* ***************************************************************************
* GLOBALS
*/

char* error_text = NULL;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

void _die(const char* file, const char* func, size_t line, const char* format, ...) {
    va_list ap;

    if(error_text) free(error_text);

    va_start(ap, format);
    vasprintf(&error_text, format, ap);
    va_end(ap);

    fprintf(stderr, "%s.%zu in %s(): %s\n", file, line, func, error_text);
    exit(1);
}


void raise(const char* format, ...) {
    va_list ap;

    if(error_text) free(error_text);

    va_start(ap, format);
    vasprintf(&error_text, format, ap);
    va_end(ap);
}


void throw(YYLTYPE* loc, const char* format, ...) {
    va_list ap;
    char* text = textat(loc);
    char* buf;

    va_start(ap, format);
    vasprintf(&buf, format, ap);
    va_end(ap);

    fprintf(stderr, "%s: %s at line %zu, col %zu\n", text, buf, loc->first_line, loc->first_column);
    free(text);
    free(buf);
    exit(1);
}
