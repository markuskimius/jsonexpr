#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eval.h"
#include "func.h"
#include "map.h"
#include "node.h"
#include "oper.h"
#include "throw.h"
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

            setmap(list, left->value.s, right);

            /* Do not free left (it's in the parse tree) */
            /* Do not free right (it's in the list) */
            break;
        }

        default:
            throw(&node->loc, "Pair expected, got %s", nodetype(node));
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
                throw(&node->loc, "Pairlist expected, got %c", nodetype(node));
                break;
        }
    }

    return list;
}


static VALUE* gettable2(SYM_TABLE* table, NODE* node) {
    VALUE* value = nullvalue();

    switch(node->type) {
        case IDENT_N:
            value = gettable(table, node->value.s);
            break;

        case '[': {
            VALUE* left = gettable2(table, node->left);
            VALUE* right = eval(node->right, table);

            if(left->type == ARRAY_V && right->type == INT_V) {
                value = getvec(left->value.v, right->value.i);
                if(!value) {
                    throw(&node->loc, "Invalid index, max %ld, got %ld", left->value.v->length-1, right->value.i);
                }
            }
            else if(left->type == OBJECT_V && right->type == STRING_V) {
                value = getmap(left->value.m, right->value.s);
                if(!value) throw(&node->loc, "Invalid key, %s", strencoded(right));
            }
            else if(left->type == ARRAY_V ) throw(&node->loc, "Array index must be INTEGER but got %s", valuetype(right));
            else if(left->type == OBJECT_V) throw(&node->loc, "Object key must be STRING but got %s", valuetype(right));
            else                            throw(&node->loc, "Array or Object expected before '[' but got %s", valuetype(left));

            freevalue(right);
            break;
        }

        case '.': {
            VALUE* left = gettable2(table, node->left);
            NODE* right = node->right;

            if     (left->type == OBJECT_V && right->type == IDENT_N) value = getmap(left->value.m, right->value.s);
            else if(left->type == OBJECT_V                          ) throw(&node->loc, "Identifier expected after '.' but got %s", nodetype(right));
            else                                                      throw(&node->loc, "Object expected before '.' but got %s", valuetype(left));

            break;
        }

        default:
            throw(&node->loc, "Invalid node type: %s", nodetype(node));
            break;
    }

    if(!value) {
        throw(&node->loc, "Invalid symbol");
    }

    return value;
}


static VALUE* settable2(SYM_TABLE* table, NODE* node, VALUE* value) {
    switch(node->type) {
        case IDENT_N:
            settable(table, node->value.s, value);
            break;

        case '[': {
            VALUE* left = gettable2(table, node->left);
            VALUE* right = eval(node->right, table);

            if(!left) {
                throw(&node->left->loc, "No such element");
            }
            else if(left->type == ARRAY_V && right->type == INT_V) {
                if(!setvec(left->value.v, right->value.i, value)) {
                    throw(&node->loc, "%s", error_text);
                }
            }
            else if(left->type == OBJECT_V && right->type == STRING_V) {
                setmap(left->value.m, right->value.s, value);
            }
            else if(left->type == ARRAY_V ) throw(&node->loc, "Array index must be INTEGER but got %s", valuetype(right));
            else if(left->type == OBJECT_V) throw(&node->loc, "Object key must be STRING but got %s", valuetype(right));
            else                            throw(&node->loc, "Array or Object expected before '[' but got %s", valuetype(left));

            freevalue(right);
            break;
        }

        case '.': {
            VALUE* left = gettable2(table, node->left);
            NODE* right = node->right;

            if     (!left                                           ) throw(&node->left->loc, "No such item");
            else if(left->type == OBJECT_V && right->type == IDENT_N) setmap(left->value.m, right->value.s, value);
            else if(left->type != OBJECT_V                          ) throw(&node->loc, "Object expected, got %s", valuetype(left));
            else                                                      throw(&node->loc, "Identifier expected, got %s", nodetype(right));

            break;
        }

        default:
            throw(&node->loc, "Invalid node type: %s", nodetype(node));
            break;
    }

    return value;
}


static VALUE* settable3(SYM_TABLE* table, NODE* node, VALUE* (*op)(VALUE*,VALUE*), VALUE* value, int ispostop) {
    VALUE* prevalue = gettable2(table, node);
    VALUE* left = dupvalue(prevalue);
    VALUE* right = value;
    VALUE* postvalue = op(left, right);
    VALUE* result = ispostop ? dupvalue(prevalue) : dupvalue(postvalue);

    swapvalue(prevalue, postvalue);
    freevalue(postvalue);

    return result;
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
    if(func && func->type == FUNCTION_V) {
        value = funcexec(func->value.fn, nodes, table);
    }
    else {
        throw(&node->left->loc, "Not a function");
    }

    /* Free arguments */
    freevec(nodes);

    /* Error handling */
    if(value == NULL) {
        throw(&node->left->loc, "%s", error_text);
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
            case BOOL_N     : result = boolvalue(node->value.i); break;
            case INT_N      : result = intvalue(node->value.i); break;
            case FLOAT_N    : result = dblvalue(node->value.f); break;
            case STRING_N   : result = strvalue(strdup(node->value.s)); break;
            case ARRAY_N    : result = arrvalue(newlist(node->left, table, NULL)); break;
            case OBJECT_N   : result = objvalue(newpairlist(node->left, table, NULL)); break;
            case CALL_N     : result = call(table, node); break;
            case SYMBOL_N   : result = dupvalue(gettable2(table, node->left)); break;
            case '='        : result = dupvalue(settable2(table, node->left, eval(node->right, table))); break;

            case PREINC_N   : result = settable3(table, node->left, op_plus, intvalue(1), 0); break;
            case PREDEC_N   : result = settable3(table, node->left, op_plus, intvalue(-1), 0); break;
            case POSTINC_N  : result = settable3(table, node->left, op_plus, intvalue(1), 1); break;
            case POSTDEC_N  : result = settable3(table, node->left, op_plus, intvalue(-1), 1); break;

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
            case POW_N      : result = op_pow(eval(node->left, table), eval(node->right, table)); break;
            case '?'        : result = op_cond(node->left, node->right, node->righter, table); break;
            case ';'        : freevalue(eval(node->left, table)); result = eval(node->right, table); break;
            default         : throw(&node->loc, "Invalid node type: %s", nodetype(node)); break;
        }
    }

    if(mytable) freetable(table);

    return result;
}
