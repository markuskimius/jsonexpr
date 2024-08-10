#include <stdio.h>
#include <stdlib.h>
#include "throw.h"
#include "token.h"


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


void throw(TOKEN* token, const char* format, ...) {
    va_list ap;

    if(error_text) free(error_text);

    va_start(ap, format);
    vasprintf(&error_text, format, ap);
    va_end(ap);

    fprintf(stderr, "%s: %s at line %d, col %d\n", token->text, error_text, token->loc.first_line, token->loc.first_column);
    exit(1);
}
