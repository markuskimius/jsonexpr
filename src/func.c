#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "eval.h"
#include "func.h"
#include "node.h"
#include "error.h"
#include "value.h"
#include "vec.h"
#include "symtbl.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

FUNC* newfunc(BINARY_FN handler, const char* name, const char* sig) {
    FUNC* func = calloc(1, sizeof(FUNC));

    func->type = BINARY_FT;
    func->handler.bin = handler;
    func->name = strdup(name);
    func->sig = strdup(sig);

    return func;
}


FUNC* newcustfunc(NODE* handler, const char* name, const char* sig, SYMTBL* ctx) {
    FUNC* func = calloc(1, sizeof(FUNC));

    func->type = CUSTOM_FT;
    func->handler.cust = handler;
    func->name = strdup(name);
    func->sig = strdup(sig);
    func->ctx = duptable(ctx);
    func->count = 1;

    return func;
}


FUNC* dupfunc(FUNC* func) {
    func->count++;

    return func;
}


void freefunc(FUNC* func) {
    func->count--;

    if(func->count == 0) {
        if(func->ctx) freetable(func->ctx);
        if(func->name) free(func->name);
        if(func->sig) free(func->sig);
        free(func);
    }
}


VEC* funcargs(const char* sig, VEC* nodes, SYMTBL* table) {
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
            throwLater("Too many arguments, expected %ld, got %zu", (cp-sig), nodes->length);
            isok = 0;
        }
        /* evaluate */
        else if(strchr("BIDSAOFf?", *cp)) {
            VALUE* v = eval(node, table);

            if(*cp != '?' && tolower(v->type) != tolower(*cp)) {
                throwLater("Invalid argument type, expected %c, got %c", *cp, v->type);
                isok = 0;
            }

            vecpush(args, v);
            cp++;
        }
        /* no evaluate */
        else if(*cp == '.') {
            /* No validation */
            vecpush(args, dupvalue(value));
            cp++;
        }
        /* many arguments */
        else if(*cp == '*') {
            if(*(cp+1) == '*') vecpush(args, dupvalue(nodes->item[i]));
            else vecpush(args, eval(node, table));

            /* do not advance cp */
        }

        if(!isok) break;
    }

    /* check too few arguments */
    if(isok && !strchr("*", *cp)) {
        throwLater("Too few arguments, expected > %ld, got %zu", (cp-sig), nodes->length);
        isok = 0;
    }

    if(!isok) {
        freevec(args);
        args = NULL;
    }

    return args;
}


VALUE* funcexec(FUNC* func, VEC* nodes, SYMTBL* table) {
    VEC* args = funcargs(func->sig, nodes, table);
    VALUE* result = NULL;

    if(args) {
        switch(func->type) {
            case BINARY_FT : {
                result = func->handler.bin(args, table);
                freevec(args);
                break;
            }

            case CUSTOM_FT : {
                SYMTBL* ctx = newtable(func->ctx);

                tableset(ctx, "ARG", arrvalue(args));
                result = eval(func->handler.cust, ctx);

                freetable(ctx);  /* also frees ARG */
                break;
            }

            default: {
                die("We shouldn't get here");
            }
        }
    }

    return result;
}


char* funcastr(FUNC* func) {
    return strdup(func->name);
}
