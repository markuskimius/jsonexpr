#include <stdio.h>
#include <stdlib.h>
#include "eval.h"
#include "func.h"
#include "map.h"
#include "node.h"
#include "oper.h"
#include "throw.h"
#include "token.h"
#include "ufunc.h"
#include "value.h"
#include "vector.h"
#include "symtable.h"


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static VEC* newlist(NODE* node, SYM_TABLE* table, VEC* list) {
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


static MAP* newpair(NODE* node, SYM_TABLE* table, MAP* list) {
    switch(node->type) {
        case ':': {
            NODE* left = node->left;
            VALUE* right = eval(node->right, table);

            setmap(list, left->token->value.s, right);

            /* Do not free left (it's in the parse tree) */
            /* Do not free right (it's in the list) */
            break;
        }

        default:
            throw(node->token, "%s: Pair expected, got '%c' (%d)\n", __FUNCTION__, node->type, node->type);
            break;
    }

    return list;
}


static MAP* newpairlist(NODE* node, SYM_TABLE* table, MAP* list) {
    if(!list) list = newmap();

    if(node) {
        switch(node->type) {
            case ',':
                if(node->left) newpair(node->left, table, list);
                if(node->right) newpairlist(node->right, table, list);
                break;

            default:
                throw(node->token, "%s: Pairlist expected, got '%c' (%d)\n", __FUNCTION__, node->type, node->type);
                break;
        }
    }

    return list;
}


static VALUE* gettable2(SYM_TABLE* table, NODE* node) {
    VALUE* value = nullvalue();

    switch(node->type) {
        case IDENT_N:
            value = gettable(table, node->token->value.s);
            break;

        case '[': {
            VALUE* left = gettable2(table, node->left);
            VALUE* right = eval(node->right, table);

            if     (left->type == ARRAY_V  && right->type == INT64_V ) value = getvec(left->value.vec, right->value.i64);
            else if(left->type == OBJECT_V && right->type == STRING_V) value = getmap(left->value.map, right->value.str);
            else if(left->type == ARRAY_V                            ) throw(node->token, "%s: Integer expected, got '%c' (%d)\n", __FUNCTION__, right->type, right->type);
            else if(left->type == OBJECT_V                           ) throw(node->token, "%s: String expected, got '%c' (%d)\n", __FUNCTION__, right->type, right->type);
            else                                                       throw(node->token, "%s: Array or object expected, got '%c' (%d)\n", __FUNCTION__, left->type, left->type);

            freevalue(right);
            break;
        }

        case '.': {
            VALUE* left = gettable2(table, node->left);
            NODE* right = node->right;

            if     (left->type == OBJECT_V && right->type == IDENT_N) value = getmap(left->value.map, right->token->value.s);
            else if(left->type == OBJECT_V                          ) throw(node->token, "%s: Identifier expected, got '%c' (%d)\n", __FUNCTION__, right->type, right->type);
            else                                                      throw(node->token, "%s: Object expected, got '%c' (%d)\n", __FUNCTION__, left->type, left->type);

            break;
        }

        default:
            throw(node->token, "%s: Invalid node type: '%c' (%d)\n", __FUNCTION__, node->type, node->type);
            break;
    }

    return value;
}


static VALUE* settable2(SYM_TABLE* table, NODE* node, VALUE* value) {
    switch(node->type) {
        case IDENT_N:
            settable(table, node->token->value.s, value);
            break;

        case '[': {
            VALUE* left = gettable2(table, node->left);
            VALUE* right = eval(node->right, table);

            if(left->type == ARRAY_V && right->type == INT64_V) {
                setvec(left->value.vec, right->value.i64, value);
            }
            else if(left->type != ARRAY_V) {
                throw(node->token, "%s: Array expected, got '%c' (%d)\n", __FUNCTION__, left->type, left->type);
            }
            else {
                throw(node->token, "%s: Integer expected, got '%c' (%d)\n", __FUNCTION__, right->type, right->type);
            }

            freevalue(right);
            break;
        }

        case '.': {
            VALUE* left = gettable2(table, node->left);
            NODE* right = node->right;

            if(left->type == OBJECT_V && right->type == IDENT_N) {
                setmap(left->value.map, right->token->value.s, value);
            }
            else if(left->type != OBJECT_V) {
                throw(node->token, "%s: Object expected, got '%c' (%d)\n", __FUNCTION__, left->type, left->type);
            }
            else {
                throw(node->token, "%s: Identifier expected, got '%c' (%d)\n", __FUNCTION__, right->type, right->type);
            }

            break;
        }

        default:
            throw(node->token, "%s: Invalid node type: '%c' (%d)\n", __FUNCTION__, node->type, node->type);
            break;
    }

    return value;
}


static void getnodelist(VEC* list, NODE* node) {
    switch(node->type) {
        case ',':
            if(node->left) getnodelist(list, node->left);
            if(node->right) getnodelist(list, node->right);
            break;

        default:
            pushvec(list, nodevalue(node));
            break;
    }
}


static VALUE* call(SYM_TABLE* table, NODE* node) {
    VALUE* func = gettable2(table, node->left);
    VALUE* value = nullvalue();
    VEC* nodes = newvec();

    /* Get arguments */
    if(node->right) getnodelist(nodes, node->right);

    /* Call the function */
    switch(func->type) {
        case BUILTIN_V  : value = funcexec(func->value.bfn, nodes, table); break;
        case USERFUNC_V : value = ufuncexec(func->value.ufn, nodes, table); break;
        default         : throw(node->left->token, "No such function"); break;
    }

    /* Free arguments */
    freevec(nodes);

    /* Error handling */
    if(value == NULL) {
        throw(node->left->token, "%s", error_text);
    }

    return value;
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VALUE* eval(NODE* node, SYM_TABLE* table) {
    VALUE* result = NULL;
    int mytable = 0;

    /* Create a symbol table if none provided */
    if(!table) {
        table = newtable(NULL);
        mytable = 1;
    }

    if(node) {
        switch(node->type) {
            case NULL_N     : result = nullvalue(); break;
            case BOOL_N     : result = boolvalue(node->token->value.i); break;
            case INT_N      : result = intvalue(node->token->value.i); break;
            case FLOAT_N    : result = dblvalue(node->token->value.f); break;
            case STRING_N   : result = strvalue(node->token->value.s); break;
            case ARRAY_N    : result = arrvalue(newlist(node->left, table, NULL)); break;
            case OBJECT_N   : result = objvalue(newpairlist(node->left, table, NULL)); break;
            case CALL_N     : result = call(table, node); break;
            case SYMBOL_N   : result = dupvalue(gettable2(table, node->left)); break;
            case '='        : result = dupvalue(settable2(table, node->left, eval(node->right, table))); break;
            case '*'        : result = op_times(eval(node->left, table), eval(node->right, table)); break;
            case '/'        : result = op_divby(eval(node->left, table), eval(node->right, table)); break;
            case '%'        : result = op_mod(eval(node->left, table), eval(node->right, table)); break;
            case '+'        : result = op_plus(eval(node->left, table), eval(node->right, table)); break;
            case '-'        : result = op_minus(eval(node->left, table), eval(node->right, table)); break;
            case EQ_N       : result = op_eq(eval(node->left, table), eval(node->right, table)); break;
            case NE_N       : result = op_ne(eval(node->left, table), eval(node->right, table)); break;
            case LT_N       : result = op_lt(eval(node->left, table), eval(node->right, table)); break;
            case LE_N       : result = op_le(eval(node->left, table), eval(node->right, table)); break;
            case GT_N       : result = op_gt(eval(node->left, table), eval(node->right, table)); break;
            case GE_N       : result = op_ge(eval(node->left, table), eval(node->right, table)); break;
            case UPLUS_N    : result = op_uplus(eval(node->left, table)); break;
            case UMINUS_N   : result = op_uminus(eval(node->left, table)); break;
            case ';'        : freevalue(eval(node->left, table)); result = eval(node->right, table); break;
            default         : throw(node->token, "%s: Invalid node type: '%c' (%d)\n", __FUNCTION__, node->type, node->type); break;
        }
    }

    if(mytable) freetable(table);

    return result;
}
