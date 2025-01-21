#define _GNU_SOURCE
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "je_error.h"
#include "je_node.h"
#include "je_util.h"
#include "je_token.h"


/* ***************************************************************************
* GLOBALS
*/

char* je_throwText = NULL;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

void _je_die(const char* file, const char* func, size_t line, const char* format, ...) {
    va_list ap;

    if(je_throwText) JE_Free(je_throwText);

    va_start(ap, format);
    vasprintf(&je_throwText, format, ap);
    va_end(ap);

    fprintf(stderr, "%s line %zu in %s(): %s\n", file, line, func, je_throwText);
    exit(1);
}


void je_throw(const char* type, JE_YYLTYPE* loc, const char* format, ...) {
    va_list ap;
    char* text = je_atextat(loc);
    char* buf;

    va_start(ap, format);
    vasprintf(&buf, format, ap);
    va_end(ap);

    fprintf(stderr, "%s at line %zu col %zu near `%s`: %s\n", type, loc->first->first_line, loc->first->first_column, text, buf);
    JE_Free(text);
    JE_Free(buf);
    exit(1);
}


void je_throwx(const char* file, const char* func, size_t line, const char* type, JE_YYLTYPE* loc, const char* format, ...) {
    va_list ap;
    char* text = je_atextat(loc);
    char* buf;

    va_start(ap, format);
    vasprintf(&buf, format, ap);
    va_end(ap);

    fprintf(stderr, "[%s:%zu@%s()] %s at line %zu col %zu near `%s`: %s\n", file, line, func, type, loc->first->first_line, loc->first->first_column, text, buf);
    JE_Free(text);
    JE_Free(buf);
    exit(1);
}


void je_throwLater(const char* format, ...) {
    va_list ap;

    if(je_throwText) JE_Free(je_throwText);

    va_start(ap, format);
    vasprintf(&je_throwText, format, ap);
    va_end(ap);
}
