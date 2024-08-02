#include <stdio.h>
#include <stdlib.h>
#include "eval.h"
#include "ast.h"
#include "ops.h"
#include "map.h"
#include "func.h"
#include "ufunc.h"
#include "value.h"
#include "vector.h"
#include "nvector.h"
#include "symtable.h"


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static VEC* newlist(AST_NODE* node, SYM_TABLE* table, VEC* list) {
    if(!list) list = newvec();

    if(node) {
        switch(node->type) {
            case ',':
                if(node->left) newlist(node->left, table, list);
                if(node->right) newlist(node->right, table, list);
                break;

            default:
                pushvec(list, eval(node, table));
                break;
        }
    }

    return list;
}


static MAP* newpair(AST_NODE* node, SYM_TABLE* table, MAP* list) {
    switch(node->type) {
        case ':': {
            AST_NODE* left = node->left;
            VALUE* right = eval(node->right, table);

            setmap(list, left->value.name, right);

            /* Do not free left (it's in the parse tree) */
            /* Do not free right (it's in the list) */
            break;
        }

        default:
            fprintf(stderr, "%s: Pair expected, got '%c' (%d)\n", __FUNCTION__, node->type, node->type);
            break;
    }

    return list;
}


static MAP* newpairlist(AST_NODE* node, SYM_TABLE* table, MAP* list) {
    if(!list) list = newmap();

    if(node) {
        switch(node->type) {
            case ',':
                if(node->left) newpair(node->left, table, list);
                if(node->right) newpairlist(node->right, table, list);
                break;

            default:
                fprintf(stderr, "%s: Pairlist expected, got '%c' (%d)\n", __FUNCTION__, node->type, node->type);
                break;
        }
    }

    return list;
}


static VALUE* gettable2(SYM_TABLE* table, AST_NODE* node) {
    VALUE* value = newnull();

    switch(node->type) {
        case IDENT_N:
            value = gettable(table, node->value.name);
            break;

        case '[': {
            VALUE* left = gettable2(table, node->left);
            VALUE* right = eval(node->right, table);

            if(left->type == ARRAY_V && right->type == INT64_V) {
                value = getvec(left->value.vec, right->value.i64);
            }
            else if(left->type != ARRAY_V) {
                fprintf(stderr, "%s: Array expected, got '%c' (%d)\n", __FUNCTION__, left->type, left->type);
            }
            else {
                fprintf(stderr, "%s: Integer expected, got '%c' (%d)\n", __FUNCTION__, right->type, right->type);
            }

            freevalue(right);
            break;
        }

        case '.': {
            VALUE* left = gettable2(table, node->left);
            AST_NODE* right = node->right;

            if(left->type == OBJECT_V && right->type == IDENT_N) {
                value = getmap(left->value.map, right->value.name);
            }
            else if(left->type != OBJECT_V) {
                fprintf(stderr, "%s: Object expected, got '%c' (%d)\n", __FUNCTION__, left->type, left->type);
            }
            else {
                fprintf(stderr, "%s: Identifier expected, got '%c' (%d)\n", __FUNCTION__, right->type, right->type);
            }

            break;
        }

        default:
            fprintf(stderr, "%s: Invalid node type: '%c' (%d)\n", __FUNCTION__, node->type, node->type);
            break;
    }

    return value;
}


static VALUE* settable2(SYM_TABLE* table, AST_NODE* node, VALUE* value) {
    switch(node->type) {
        case IDENT_N:
            settable(table, node->value.name, value);
            break;

        case '[': {
            VALUE* left = gettable2(table, node->left);
            VALUE* right = eval(node->right, table);

            if(left->type == ARRAY_V && right->type == INT64_V) {
                setvec(left->value.vec, right->value.i64, value);
            }
            else if(left->type != ARRAY_V) {
                fprintf(stderr, "%s: Array expected, got '%c' (%d)\n", __FUNCTION__, left->type, left->type);
            }
            else {
                fprintf(stderr, "%s: Integer expected, got '%c' (%d)\n", __FUNCTION__, right->type, right->type);
            }

            freevalue(right);
            break;
        }

        case '.': {
            VALUE* left = gettable2(table, node->left);
            AST_NODE* right = node->right;

            if(left->type == OBJECT_V && right->type == IDENT_N) {
                setmap(left->value.map, right->value.name, value);
            }
            else if(left->type != OBJECT_V) {
                fprintf(stderr, "%s: Object expected, got '%c' (%d)\n", __FUNCTION__, left->type, left->type);
            }
            else {
                fprintf(stderr, "%s: Identifier expected, got '%c' (%d)\n", __FUNCTION__, right->type, right->type);
            }

            break;
        }

        default:
            fprintf(stderr, "%s: Invalid node type: '%c' (%d)\n", __FUNCTION__, node->type, node->type);
            break;
    }

    return value;
}


static void getnodelist(NVEC* nvec, AST_NODE* node) {
    switch(node->type) {
        case ',':
            if(node->left) getnodelist(nvec, node->left);
            if(node->right) getnodelist(nvec, node->right);
            break;

        default:
            pushnvec(nvec, node);
            break;
    }
}


static VALUE* fncall(SYM_TABLE* table, AST_NODE* node) {
    VALUE* func = gettable2(table, node->left);
    VALUE* value = newnull();
    NVEC* nvec = newnvec();

    /* Get arguments */
    if(node->right) getnodelist(nvec, node->right);

    /* Call the function */
    switch(func->type) {
        case BUILTIN_V  : value = funcexec(func->value.bfn, nvec->length, nvec->item, table); break;
        case USERFUNC_V : value = ufuncexec(func->value.ufn, nvec->length, nvec->item, table); break;
        default         : fprintf(stderr, "%s: Invalid value type: '%c' (%d)\n", __FUNCTION__, func->type, func->type); break;
    }

    /* Free arguments */
    freenvec(nvec);

    return value;
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VALUE* eval(AST_NODE* node, SYM_TABLE* table) {
    VALUE* result = NULL;

    /* Create a symbol table if none provided */
    if(!table) table = newtable(NULL);

    if(node) {
        switch(node->type) {
            case INT64_N    : result = newint64(node->value.i64); break;
            case DOUBLE_N   : result = newdouble(node->value.f64); break;
            case STRING_N   : result = newstring(node->value.str); break;
            case ARRAY_N    : result = newarray(newlist(node->left, table, NULL)); break;
            case OBJECT_N   : result = newobject(newpairlist(node->left, table, NULL)); break;
            case LVALUE_N   : result = dupvalue(gettable2(table, node->left)); break;
            case '='        : result = dupvalue(settable2(table, node->left, eval(node->right, table))); break;
            case FNCALL_N   : result = dupvalue(fncall(table, node)); break;
            case '*'        : result = op_times(eval(node->left, table), eval(node->right, table)); break;
            case '/'        : result = op_divby(eval(node->left, table), eval(node->right, table)); break;
            case '%'        : result = op_mod(eval(node->left, table), eval(node->right, table)); break;
            case '+'        : result = op_plus(eval(node->left, table), eval(node->right, table)); break;
            case '-'        : result = op_minus(eval(node->left, table), eval(node->right, table)); break;
            case UPLUS_N    : result = op_uplus(eval(node->left, table)); break;
            case UMINUS_N   : result = op_uminus(eval(node->left, table)); break;
            case ';'        : freevalue(eval(node->left, table)); result = eval(node->right, table); break;
            default         : fprintf(stderr, "%s: Invalid node type: '%c' (%d)\n", __FUNCTION__, node->type, node->type); break;
        }
    }

    return result;
}
