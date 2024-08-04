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

UFUNC* newufunc(NODE* handler, const char* name, const char* sig) {
    UFUNC* ufunc = calloc(1, sizeof(UFUNC));

    ufunc->handler = handler;
    ufunc->name = strdup(name);
    ufunc->sig = strdup(sig);

    return ufunc;
}


void freeufunc(UFUNC* ufunc) {
    free(ufunc->name);
    free(ufunc->sig);
    free(ufunc);
}


char* astrufunc(UFUNC* ufunc) {
    return strdup(ufunc->name);
}


VALUE* ufuncexec(UFUNC* ufunc, VEC* nodes, SYM_TABLE* table) {
    VEC* args = funcargs(ufunc->sig, nodes, table);
    VALUE* result = newnull();

    if(args) {
        settable(table, "ARG", newarray(args));

        result = eval(ufunc->handler, table);

        unsettable(table, "ARG");
        /* unsettable handles free */
    }

    return result;
}
