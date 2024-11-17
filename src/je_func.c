#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "je_error.h"
#include "je_eval.h"
#include "je_func.h"
#include "je_symtbl.h"
#include "je_val.h"
#include "je_vec.h"
#include "je_node.h"
#include "je_map.h"
#include "je_util.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_FUNC* je_newfunc(JE_BINARY_FN handler, const char* name, const char* sig) {
    JE_FUNC* func = calloc(1, sizeof(JE_FUNC));

    func->type = JE_BINARY_FT;
    func->handler.bin = handler;
    func->name = strdup(name);
    func->sig = strdup(sig);
    func->count = 1;

    return func;
}


JE_FUNC* je_newcustfunc(JE_NODE* handler, const char* name, const char* sig, JE_SYMTBL* ctx) {
    JE_FUNC* func = calloc(1, sizeof(JE_FUNC));

    func->type = JE_CUSTOM_FT;
    func->handler.cust = handler;
    func->name = strdup(name);
    func->sig = strdup(sig);
    func->ctx = je_duptable(ctx);
    func->count = 1;

    return func;
}


JE_FUNC* je_dupfunc(JE_FUNC* func) {
    func->count++;

    return func;
}


void je_freefunc(JE_FUNC* func) {
    func->count--;

    if(func->count == 0) {
        if(func->ctx) je_freetable(func->ctx);
        if(func->name) free(func->name);
        if(func->sig) free(func->sig);
        free(func);
    }
}


JE_VEC* je_funcargs(const char* sig, JE_VEC* nodes, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VEC* args = je_newvec();
    const char* cp = sig;
    int isok = 1;

    for(size_t i=0; i<nodes->length; i++) {
        JE_VAL* val = nodes->item[i];
        JE_NODE* node = val->value.n;

        /* val must be a node */
        assert(val->type == JE_NODE_V);

        /* check too many arguments */
        if(*cp == '\0') {
            JeRuntimeError(&node->loc, "Too many arguments, expected %ld, got %zu", (cp-sig), nodes->length);
            isok = 0;
        }
        /* evaluate */
        else if(strchr("BIDSAOF#@?", *cp)) {
            JE_VAL* v = je_eval(node, table);

            if(*cp=='?' || *cp==v->type || (*cp=='#'&&strchr("ID",v->type)) || (*cp=='@'&&strchr("AO",v->type))) {
                /* Same type -- ok */
            }
            else if(*cp=='S' && v->type==JE_NODE_V) {
                /* Node type where string is expected -- convert node to string */
                JE_VAL* sv = je_strval(je_valstr(v));

                je_freeval(v);
                v = sv;
            }
            else {
                JeRuntimeError(&node->loc, "%d-th argument is invalid type, expected %c, got %c", (int)i, *cp, v->type);
                isok = 0;
            }

            je_vecpush(args, v);
            cp++;
        }
        /* no evaluate */
        else if(*cp == '.') {
            /* No validation */
            je_vecpush(args, je_dupval(val));
            cp++;
        }
        /* many arguments */
        else if(*cp == '*') {
            if(*(cp+1) == '*') je_vecpush(args, je_dupval(nodes->item[i]));
            else je_vecpush(args, je_eval(node, table));

            /* do not advance cp */
        }

        if(!isok) break;
    }

    /* check too few arguments */
    if(isok && !strchr("*", *cp)) {
        JeRuntimeError(loc, "Too few arguments, expected > %ld, got %zu", (cp-sig), nodes->length);
        isok = 0;
    }

    if(!isok) {
        je_freevec(args);
        args = NULL;
    }

    return args;
}


JE_VAL* je_funcexec(JE_FUNC* func, JE_VEC* nodes, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VEC* args = je_funcargs(func->sig, nodes, table, loc);
    JE_VAL* result = NULL;

    if(args) {
        switch(func->type) {
            case JE_BINARY_FT : {
                result = func->handler.bin(args, table, loc);

                je_freevec(args);
                break;
            }

            case JE_CUSTOM_FT : {
                JE_SYMTBL* ctx = je_newtable(func->ctx);

                je_tableset(ctx, "ARG", je_arrval(args), 1);
                result = je_eval(func->handler.cust, ctx);

                je_freetable(ctx);  /* also frees ARG */
                break;
            }

            default: {
                je_die("We shouldn't get here");
            }
        }
    }

    return result;
}


char* je_funcastr(JE_FUNC* func) {
    char buf[64];

    snprintf(buf, sizeof(buf), "FUNCTION##%s()", func->name);

    return strdup(buf);
}
