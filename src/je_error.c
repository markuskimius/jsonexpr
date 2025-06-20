#define _GNU_SOURCE
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "je_ast.h"
#include "je_error.h"
#include "je_token.h"
#include "je_util.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

void JE_Error(const char* type, const JE_TOKEN* begin, const JE_TOKEN* end, const char* format, ...) {
    va_list ap;
    char* text = JE_TokenRangeToCstr(begin, end);
    char* buf;

    va_start(ap, format);
    vasprintf(&buf, format, ap);
    va_end(ap);

    fprintf(stderr, "%s at line %zu col %zu near `%s`: %s\n", type, begin->first_line, begin->first_column, text, buf);

    JE_Free(text);
    JE_Free(buf);
}
