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

char* JE_throwText = NULL;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

void _JE_Die(const char* file, const char* func, size_t line, const char* format, ...) {
    va_list ap;

    if(JE_throwText) free(JE_throwText);

    va_start(ap, format);
    vasprintf(&JE_throwText, format, ap);
    va_end(ap);

    fprintf(stderr, "%s line %zu in %s(): %s\n", file, line, func, JE_throwText);
    exit(1);
}

void JE_Throw(const char* type, JE_YYLTYPE* loc, const char* format, ...) {
    va_list ap;
    char* text = JE_LocToAstr(loc);
    char* buf;

    va_start(ap, format);
    vasprintf(&buf, format, ap);
    va_end(ap);

    fprintf(stderr, "%s at line %zu col %zu near `%s`: %s\n", type, loc->first->first_line, loc->first->first_column, text, buf);
    free(text);
    free(buf);
    exit(1);
}

void JE_Throwx(const char* file, const char* func, size_t line, const char* type, JE_YYLTYPE* loc, const char* format, ...) {
    va_list ap;
    char* text = JE_LocToAstr(loc);
    char* buf;

    va_start(ap, format);
    vasprintf(&buf, format, ap);
    va_end(ap);

    fprintf(stderr, "[%s:%zu@%s()] %s at line %zu col %zu near `%s`: %s\n", file, line, func, type, loc->first->first_line, loc->first->first_column, text, buf);
    free(text);
    free(buf);
    exit(1);
}

void JE_ThrowLater(const char* format, ...) {
    va_list ap;

    if(JE_throwText) free(JE_throwText);

    va_start(ap, format);
    vasprintf(&JE_throwText, format, ap);
    va_end(ap);
}
