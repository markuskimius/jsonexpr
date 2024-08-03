#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func.h"
#include "node.h"
#include "value.h"
#include "symtable.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

FUNC* newfunc(FUNC_HANDLER handler, const char* name, const char* sig) {
    FUNC* func = calloc(1, sizeof(FUNC));

    func->handler = handler;
    func->name = strdup(name);
    func->sig = strdup(sig);

    return func;
}


void freefunc(FUNC* func) {
    free(func->name);
    free(func->sig);
    free(func);
}


char* astrfunc(FUNC* func) {
    return strdup(func->name);
}


VALUE* funcexec(FUNC* func, size_t argc, NODE** argv, SYM_TABLE* table) {
    VALUE* value = newnull();

/* TODO validate */
    value = func->handler(argc, argv, table);

    return value;
}
