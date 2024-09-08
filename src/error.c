#define _GNU_SOURCE
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "node.h"
#include "util.h"


/* ***************************************************************************
* GLOBALS
*/

char* throwText = NULL;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

void _die(const char* file, const char* func, size_t line, const char* format, ...) {
    va_list ap;

    if(throwText) free(throwText);

    va_start(ap, format);
    vasprintf(&throwText, format, ap);
    va_end(ap);

    fprintf(stderr, "%s line %zu in %s(): %s\n", file, line, func, throwText);
    exit(1);
}


void throw(const char* type, YYLTYPE* loc, const char* format, ...) {
    va_list ap;
    char* text = textat(loc);
    char* buf;

    va_start(ap, format);
    vasprintf(&buf, format, ap);
    va_end(ap);

    fprintf(stderr, "%s at line %zu col %zu near `%s`: %s\n", type, loc->first_line, loc->first_column, text, buf);
    free(text);
    free(buf);
    exit(1);
}


void throwx(const char* file, const char* func, size_t line, const char* type, YYLTYPE* loc, const char* format, ...) {
    va_list ap;
    char* text = textat(loc);
    char* buf;

    va_start(ap, format);
    vasprintf(&buf, format, ap);
    va_end(ap);

    fprintf(stderr, "[%s:%zu@%s()] %s at line %zu col %zu near `%s`: %s\n", file, line, func, type, loc->first_line, loc->first_column, text, buf);
    free(text);
    free(buf);
    exit(1);
}


void throwLater(const char* format, ...) {
    va_list ap;

    if(throwText) free(throwText);

    va_start(ap, format);
    vasprintf(&throwText, format, ap);
    va_end(ap);
}
