#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eval.h"
#include "func.h"
#include "node.h"
#include "map.h"
#include "util.h"
#include "ufunc.h"
#include "value.h"
#include "vector.h"
#include "symtable.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

UFUNC* newufunc(NODE* handler, const char* name, const char* sig, SYM_TABLE* ctx) {
    UFUNC* ufunc = calloc(1, sizeof(UFUNC));

    ufunc->handler = handler;
    ufunc->name = strdup(name);
    ufunc->sig = strdup(sig);
    ufunc->ctx = duptable(ctx);

    return ufunc;
}


void freeufunc(UFUNC* ufunc) {
    freetable(ufunc->ctx);
    free(ufunc->name);
    free(ufunc->sig);
    free(ufunc);
}


char* astrufunc(UFUNC* ufunc) {
    return strdup(ufunc->name);
}


VALUE* ufuncexec(UFUNC* ufunc, VEC* nodes, SYM_TABLE* table) {
    VEC* args = funcargs(ufunc->sig, nodes, table);
    VALUE* result = NULL;

    if(args) {
        SYM_TABLE* ctx = newtable(ufunc->ctx);

        settable(ctx, "ARG", arrvalue(args));
        result = eval(ufunc->handler, ctx);

        freetable(ctx);
    }

    return result;
}
