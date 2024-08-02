#include <stdlib.h>
#include <string.h>
#include "ufunc.h"
#include "ast.h"
#include "eval.h"
#include "util.h"
#include "value.h"
#include "symtable.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

UFUNC* newufunc(const char* name, AST_NODE* handler, size_t minargs, size_t maxargs) {
    UFUNC* ufunc = calloc(1, sizeof(UFUNC));

    ufunc->handler = handler;
    ufunc->minargs = minargs;
    ufunc->maxargs = maxargs;
    ufunc->name = strdup(name);

    return ufunc;
}


void freeufunc(UFUNC* ufunc) {
    free(ufunc->name);
    free(ufunc);
}


char* astrufunc(UFUNC* ufunc) {
    return strdup(ufunc->name);
}


VALUE* ufuncexec(UFUNC* ufunc, size_t argc, AST_NODE** argv, SYM_TABLE* table) {
/* TODO */
    return eval(ufunc->handler, table);
}
