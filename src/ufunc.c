#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eval.h"
#include "node.h"
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


VALUE* ufuncexec(UFUNC* ufunc, VEC* args, SYM_TABLE* table) {
    VALUE* value = newnull();

/* TODO - Validate signature */
    value = eval(ufunc->handler, table);

    return value;
}
