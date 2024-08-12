#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "eval.h"
#include "func.h"
#include "node.h"
#include "throw.h"
#include "value.h"
#include "vector.h"
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


VEC* funcargs(const char* sig, VEC* nodes, SYM_TABLE* table) {
    VEC* args = newvec();
    const char* cp = sig;
    int isok = 1;

    for(size_t i=0; i<nodes->length; i++) {
        VALUE* value = nodes->item[i];
        NODE* node = value->value.n;

        /* value must be a node */
        assert(value->type == NODE_V);

        /* check too many arguments */
        if(*cp == '\0') {
            raise("Too many arguments, expected %ld, got %zu", (cp-sig), nodes->length);
            isok = 0;
        }
        /* evaluate */
        else if(strchr("BIDSAOFf?", *cp)) {
            VALUE* v = eval(node, table);

            if(*cp != '?' && tolower(v->type) != tolower(*cp)) {
                raise("Invalid argument type, expected %c, got %c", *cp, v->type);
                isok = 0;
            }

            pushvec(args, v);
            cp++;
        }
        /* no evaluate */
        else if(*cp == '.') {
            /* No validation */
            pushvec(args, dupvalue(value));
            cp++;
        }
        /* many arguments */
        else if(*cp == '*') {
            if(*(cp+1) == '*') pushvec(args, dupvalue(nodes->item[i]));
            else pushvec(args, eval(node, table));

            /* do not advance cp */
        }

        if(!isok) break;
    }

    /* check too few arguments */
    if(isok && !strchr("*", *cp)) {
        raise("Too few arguments, expected > %ld, got %zu", (cp-sig), nodes->length);
        isok = 0;
    }

    if(!isok) {
        freevec(args);
        args = NULL;
    }

    return args;
}


VALUE* funcexec(FUNC* func, VEC* nodes, SYM_TABLE* table) {
    VEC* args = funcargs(func->sig, nodes, table);
    VALUE* result = NULL;

    if(args) {
        result = func->handler(args, table);
        freevec(args);
    }

    return result;
}
