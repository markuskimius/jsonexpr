#include <stdlib.h>
#include <string.h>
#include "func.h"
#include "ast.h"
#include "value.h"
#include "symtable.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

FUNC* newfunc(const char* name, FUNC_HANDLER handler, size_t minargs, size_t maxargs) {
    FUNC* func = calloc(1, sizeof(FUNC));

    func->handler = handler;
    func->minargs = minargs;
    func->maxargs = maxargs;
    func->name = strdup(name);

    return func;
}


void freefunc(FUNC* func) {
    free(func->name);
    free(func);
}


char* astrfunc(FUNC* func) {
    return strdup(func->name);
}


VALUE* funcexec(FUNC* func, size_t argc, AST_NODE** argv, SYM_TABLE* table) {
/* TODO */
    return func->handler(argc, argv, table);
}
