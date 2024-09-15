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


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static JE_VAL* tablemod(JE_SYMTBL* table, JE_NODE* node) {
    JE_VAL* val = NULL;

    switch(node->type) {
        case JE_IDENT_N:
            val = je_tableget(table, node->value.s);

            if(!val) {
                val = je_nullval();
                je_tableset(table, node->value.s, val);
            }

            break;

        case '[': {
            JE_VAL* left = tablemod(table, node->left);
            JE_VAL* right = je_eval(node->right, table);

            if(left->type == JE_ARRAY_V && right->type == JE_INT_V) {
                val = je_vecget(left->value.v, right->value.i);
                if(!val) {
                    JeRuntimeError(&node->loc, "Invalid index, max %ld, got %ld", left->value.v->length-1, right->value.i);
                }
            }
            else if(left->type == JE_OBJECT_V && right->type == JE_STRING_V) {
                val = je_mapget(left->value.m, right->value.s);
                if(!val) JeRuntimeError(&node->loc, "Invalid key, %s", je_valqstr(right));
            }
            else if(left->type == JE_ARRAY_V ) JeRuntimeError(&node->loc, "ARRAY index must be INTEGER but got %s", je_valtype(right));
            else if(left->type == JE_OBJECT_V) JeRuntimeError(&node->loc, "OBJECT key must be STRING but got %s", je_valtype(right));
            else                               JeRuntimeError(&node->loc, "ARRAY or OBJECT expected before '[' but got %s", je_valtype(left));

            je_freeval(right);
            break;
        }

        case '.': {
            JE_VAL* left = tablemod(table, node->left);
            JE_NODE* right = node->right;

            if     (left->type == JE_OBJECT_V && right->type == JE_IDENT_N) val = je_mapget(left->value.m, right->value.s);
            else if(left->type == JE_OBJECT_V                             ) JeRuntimeError(&node->loc, "IDENTIFIER expected after '.' but got %s", je_nodetype(right));
            else                                                            JeRuntimeError(&node->loc, "OBJECT expected before '.' but got %s", je_valtype(left));

            break;
        }

        default:
            JeParseError(&node->loc, "Invalid node type: %s", je_nodetype(node));
            break;
    }

    if(!val) {
        JeRuntimeError(&node->loc, "Undefined symbol");
    }

    return val ? val : je_nullval();
}


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


JE_VEC* je_funcargs(const char* sig, JE_VEC* nodes, JE_SYMTBL* table) {
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
            je_throwLater("Too many arguments, expected %ld, got %zu", (cp-sig), nodes->length);
            isok = 0;
        }
        /* evaluate */
        else if(strchr("BIDSAOF#?", *cp)) {
            JE_VAL* v = je_eval(node, table);

            if(*cp=='?' || *cp==v->type || (*cp=='#'&&strchr("ID",v->type))) {
                /* ok */
            }
            else {
                je_throwLater("Invalid argument type, expected %c, got %c", *cp, v->type);
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
        je_throwLater("Too few arguments, expected > %ld, got %zu", (cp-sig), nodes->length);
        isok = 0;
    }

    if(!isok) {
        je_freevec(args);
        args = NULL;
    }

    return args;
}


JE_VAL* je_funcexec(JE_FUNC* func, JE_VEC* nodes, JE_SYMTBL* table) {
    JE_VEC* args = je_funcargs(func->sig, nodes, table);
    JE_VAL* result = NULL;

    if(args) {
        switch(func->type) {
            case JE_BINARY_FT : {
                result = func->handler.bin(args, table);
                je_freevec(args);
                break;
            }

            case JE_CUSTOM_FT : {
                JE_SYMTBL* ctx = je_newtable(func->ctx);

                je_tableset(ctx, "ARG", je_arrval(args));
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
    return strdup(func->name);
}
