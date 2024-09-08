#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "eval.h"
#include "func.h"
#include "symtbl.h"
#include "val.h"
#include "vec.h"
#include "node.h"
#include "map.h"


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static VAL* tablemod(SYMTBL* table, NODE* node) {
    VAL* val = NULL;

    switch(node->type) {
        case IDENT_N:
            val = tableget(table, node->value.s);

            if(!val) {
                val = nullval();
                tableset(table, node->value.s, val);
            }

            break;

        case '[': {
            VAL* left = tablemod(table, node->left);
            VAL* right = eval(node->right, table);

            if(left->type == ARRAY_V && right->type == INT_V) {
                val = vecget(left->value.v, right->value.i);
                if(!val) {
                    RuntimeError(&node->loc, "Invalid index, max %ld, got %ld", left->value.v->length-1, right->value.i);
                }
            }
            else if(left->type == OBJECT_V && right->type == STRING_V) {
                val = mapget(left->value.m, right->value.s);
                if(!val) RuntimeError(&node->loc, "Invalid key, %s", valqstr(right));
            }
            else if(left->type == ARRAY_V ) RuntimeError(&node->loc, "ARRAY index must be INTEGER but got %s", valtype(right));
            else if(left->type == OBJECT_V) RuntimeError(&node->loc, "OBJECT key must be STRING but got %s", valtype(right));
            else                            RuntimeError(&node->loc, "ARRAY or OBJECT expected before '[' but got %s", valtype(left));

            freeval(right);
            break;
        }

        case '.': {
            VAL* left = tablemod(table, node->left);
            NODE* right = node->right;

            if     (left->type == OBJECT_V && right->type == IDENT_N) val = mapget(left->value.m, right->value.s);
            else if(left->type == OBJECT_V                          ) RuntimeError(&node->loc, "IDENTIFIER expected after '.' but got %s", nodetype(right));
            else                                                      RuntimeError(&node->loc, "OBJECT expected before '.' but got %s", valtype(left));

            break;
        }

        default:
            ParseError(&node->loc, "Invalid node type: %s", nodetype(node));
            break;
    }

    if(!val) {
        RuntimeError(&node->loc, "Undefined symbol");
    }

    return val ? val : nullval();
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

FUNC* newfunc(BINARY_FN handler, const char* name, const char* sig) {
    FUNC* func = calloc(1, sizeof(FUNC));

    func->type = BINARY_FT;
    func->handler.bin = handler;
    func->name = strdup(name);
    func->sig = strdup(sig);
    func->count = 1;

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
        VAL* val = nodes->item[i];
        NODE* node = val->value.n;

        /* val must be a node */
        assert(val->type == NODE_V);

        /* check too many arguments */
        if(*cp == '\0') {
            throwLater("Too many arguments, expected %ld, got %zu", (cp-sig), nodes->length);
            isok = 0;
        }
        /* evaluate */
        else if(strchr("BIDSAOF#?", *cp)) {
            VAL* v = eval(node, table);

            if(*cp=='?' || *cp==v->type || (*cp=='#'&&strchr("ID",v->type))) {
                /* ok */
            }
            else {
                throwLater("Invalid argument type, expected %c, got %c", *cp, v->type);
                isok = 0;
            }

            vecpush(args, v);
            cp++;
        }
        /* no evaluate */
        else if(*cp == '.') {
            /* No validation */
            vecpush(args, dupval(val));
            cp++;
        }
        /* many arguments */
        else if(*cp == '*') {
            if(*(cp+1) == '*') vecpush(args, dupval(nodes->item[i]));
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


VAL* funcexec(FUNC* func, VEC* nodes, SYMTBL* table) {
    VEC* args = funcargs(func->sig, nodes, table);
    VAL* result = NULL;

    if(args) {
        switch(func->type) {
            case BINARY_FT : {
                result = func->handler.bin(args, table);
                freevec(args);
                break;
            }

            case CUSTOM_FT : {
                SYMTBL* ctx = newtable(func->ctx);

                tableset(ctx, "ARG", arrval(args));
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
