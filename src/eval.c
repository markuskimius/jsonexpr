#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eval.h"
#include "func.h"
#include "map.h"
#include "node.h"
#include "oper.h"
#include "error.h"
#include "value.h"
#include "vec.h"
#include "symtbl.h"


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static VEC* newlist(NODE* node, SYMTBL* table, VEC* list) {
    if(!list) list = newvec();

    if(node) {
        switch(node->type) {
            case ',':
                if(node->left) newlist(node->left, table, list);
                if(node->right) newlist(node->right, table, list);
                break;

            default:
                vecpush(list, eval(node, table));
                break;
        }
    }

    return list;
}


static MAP* newpair(NODE* node, SYMTBL* table, MAP* list) {
    switch(node->type) {
        case ':': {
            NODE* left = node->left;
            VALUE* right = eval(node->right, table);

            if(left->type == STRING_N) mapset(list, left->value.s, right);
            else {
                freevalue(right);
                RuntimeError(&left->loc, "STRING expected, got %s", nodetype(left));
            }

            /* Do not free left (it's in the parse tree) */
            /* Do not free right (it's in the list) */
            break;
        }

        default:
            RuntimeError(&node->loc, "PAIR expected, got %s", nodetype(node));
            break;
    }

    return list;
}


static MAP* newpairlist(NODE* node, SYMTBL* table, MAP* list) {
    if(!list) list = newmap();

    if(node) {
        switch(node->type) {
            case ',':
                if(node->left) newpair(node->left, table, list);
                if(node->right) newpairlist(node->right, table, list);
                break;

            default:
                RuntimeError(&node->loc, "PAIRLIST expected, got %c", nodetype(node));
                break;
        }
    }

    return list;
}


static VALUE* tableget2(SYMTBL* table, NODE* node) {
    VALUE* value = nullvalue();

    switch(node->type) {
        case IDENT_N:
            value = tableget(table, node->value.s);
            break;

        case '[': {
            VALUE* left = tableget2(table, node->left);
            VALUE* right = eval(node->right, table);

            if(left->type == ARRAY_V && right->type == INT_V) {
                value = vecget(left->value.v, right->value.i);
                if(!value) {
                    RuntimeError(&node->loc, "Invalid index, max %ld, got %ld", left->value.v->length-1, right->value.i);
                }
            }
            else if(left->type == OBJECT_V && right->type == STRING_V) {
                value = mapget(left->value.m, right->value.s);
                if(!value) RuntimeError(&node->loc, "Invalid key, %s", valueqstr(right));
            }
            else if(left->type == ARRAY_V ) RuntimeError(&node->loc, "ARRAY index must be INTEGER but got %s", valuetype(right));
            else if(left->type == OBJECT_V) RuntimeError(&node->loc, "OBJECT key must be STRING but got %s", valuetype(right));
            else                            RuntimeError(&node->loc, "ARRAY or OBJECT expected before '[' but got %s", valuetype(left));

            freevalue(right);
            break;
        }

        case '.': {
            VALUE* left = tableget2(table, node->left);
            NODE* right = node->right;

            if     (left->type == OBJECT_V && right->type == IDENT_N) value = mapget(left->value.m, right->value.s);
            else if(left->type == OBJECT_V                          ) RuntimeError(&node->loc, "IDENTIFIER expected after '.' but got %s", nodetype(right));
            else                                                      RuntimeError(&node->loc, "OBJECT expected before '.' but got %s", valuetype(left));

            break;
        }

        default:
            ParseError(&node->loc, "Invalid node type: %s", nodetype(node));
            break;
    }

    if(!value) {
        RuntimeError(&node->loc, "Undefined symbol");
    }

    return value;
}


static VALUE* tableset2(SYMTBL* table, NODE* node, VALUE* value) {
    switch(node->type) {
        case IDENT_N:
            tableset(table, node->value.s, value);
            break;

        case '[': {
            VALUE* left = tableget2(table, node->left);
            VALUE* right = eval(node->right, table);

            if(!left) {
                RuntimeError(&node->left->loc, "No such element");
            }
            else if(left->type == ARRAY_V && right->type == INT_V) {
                if(!vecset(left->value.v, right->value.i, value)) {
                    RuntimeError(&node->loc, "%s", throwText);
                }
            }
            else if(left->type == OBJECT_V && right->type == STRING_V) {
                mapset(left->value.m, right->value.s, value);
            }
            else if(left->type == ARRAY_V ) RuntimeError(&node->loc, "ARRAY index must be INTEGER but got %s", valuetype(right));
            else if(left->type == OBJECT_V) RuntimeError(&node->loc, "OBJECT key must be STRING but got %s", valuetype(right));
            else                            RuntimeError(&node->loc, "ARRAY or OBJECT expected before '[' but got %s", valuetype(left));

            freevalue(right);
            break;
        }

        case '.': {
            VALUE* left = tableget2(table, node->left);
            NODE* right = node->right;

            if     (!left                                           ) RuntimeError(&node->left->loc, "No such item");
            else if(left->type == OBJECT_V && right->type == IDENT_N) mapset(left->value.m, right->value.s, value);
            else if(left->type != OBJECT_V                          ) RuntimeError(&node->loc, "OBJECT expected, got %s", valuetype(left));
            else                                                      RuntimeError(&node->loc, "IDENTIFIER expected, got %s", nodetype(right));

            break;
        }

        default:
            ParseError(&node->loc, "Invalid node type: %s", nodetype(node));
            break;
    }

    return value;
}


static VALUE* tableset3(SYMTBL* table, NODE* node, VALUE* (*op)(VALUE*,VALUE*), VALUE* value, int ispostop) {
    VALUE* prevalue = tableget2(table, node);
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
            vecpush(list, nodevalue(node));
            break;
    }
}


static VALUE* call(SYMTBL* table, NODE* node) {
    VALUE* func = tableget2(table, node->left);
    VALUE* value = nullvalue();
    VEC* nodes = newvec();

    /* Get arguments */
    if(node->right) getnodelist(nodes, node->right);

    /* Call the function */
    if(func && func->type == FUNCTION_V) {
        value = funcexec(func->value.fn, nodes, table);
    }
    else {
        RuntimeError(&node->left->loc, "Not a function");
    }

    /* Free arguments */
    freevec(nodes);

    /* Error handling */
    if(value == NULL) {
        RuntimeError(&node->left->loc, "%s", throwText);
    }

    return value;
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VALUE* eval(NODE* node, SYMTBL* table) {
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
            case SYMBOL_N   : result = dupvalue(tableget2(table, node->left)); break;
            case '='        : result = dupvalue(tableset2(table, node->left, eval(node->right, table))); break;

            case PREINC_N   : result = tableset3(table, node->left,  op_plus,  intvalue(1), 0); break;
            case PREDEC_N   : result = tableset3(table, node->left, op_minus, intvalue(-1), 0); break;
            case POSTINC_N  : result = tableset3(table, node->left,  op_plus,  intvalue(1), 1); break;
            case POSTDEC_N  : result = tableset3(table, node->left, op_minus, intvalue(-1), 1); break;

            case PLEQ_N     : result = tableset3(table, node->left,  op_plus, eval(node->right, table), 0); break;
            case MIEQ_N     : result = tableset3(table, node->left, op_minus, eval(node->right, table), 0); break;
            case TIEQ_N     : result = tableset3(table, node->left, op_times, eval(node->right, table), 0); break;
            case DIEQ_N     : result = tableset3(table, node->left, op_divby, eval(node->right, table), 0); break;
            case MOEQ_N     : result = tableset3(table, node->left,   op_mod, eval(node->right, table), 0); break;
            case SHLEQ_N    : result = tableset3(table, node->left,   op_shl, eval(node->right, table), 0); break;
            case ASREQ_N    : result = tableset3(table, node->left,   op_asr, eval(node->right, table), 0); break;
            case SHREQ_N    : result = tableset3(table, node->left,   op_shr, eval(node->right, table), 0); break;
            case ANDEQ_N    : result = tableset3(table, node->left,  op_band, eval(node->right, table), 0); break;
            case XOREQ_N    : result = tableset3(table, node->left,  op_bxor, eval(node->right, table), 0); break;
            case OREQ_N     : result = tableset3(table, node->left,   op_bor, eval(node->right, table), 0); break;
            case POWEQ_N    : result = tableset3(table, node->left,   op_pow, eval(node->right, table), 0); break;

            case '*'        : result = op_times(eval(node->left, table), eval(node->right, table)); break;
            case '/'        : result = op_divby(eval(node->left, table), eval(node->right, table)); break;
            case '%'        : result = op_mod(eval(node->left, table), eval(node->right, table)); break;
            case '+'        : result = op_plus(eval(node->left, table), eval(node->right, table)); break;
            case '-'        : result = op_minus(eval(node->left, table), eval(node->right, table)); break;
            case '|'        : result = op_bor(eval(node->left, table), eval(node->right, table)); break;
            case '&'        : result = op_band(eval(node->left, table), eval(node->right, table)); break;
            case '^'        : result = op_bxor(eval(node->left, table), eval(node->right, table)); break;
            case '~'        : result = op_bnot(eval(node->left, table)); break;
            case '!'        : result = op_lnot(eval(node->left, table)); break;
            case EQ_N       : result = op_eq(eval(node->left, table), eval(node->right, table)); break;
            case NE_N       : result = op_ne(eval(node->left, table), eval(node->right, table)); break;
            case LT_N       : result = op_lt(eval(node->left, table), eval(node->right, table)); break;
            case LE_N       : result = op_le(eval(node->left, table), eval(node->right, table)); break;
            case GT_N       : result = op_gt(eval(node->left, table), eval(node->right, table)); break;
            case GE_N       : result = op_ge(eval(node->left, table), eval(node->right, table)); break;
            case OR_N       : result = op_lor(node->left, node->right, table); break;
            case AND_N      : result = op_land(node->left, node->right, table); break;
            case UPLUS_N    : result = op_uplus(eval(node->left, table)); break;
            case UMINUS_N   : result = op_uminus(eval(node->left, table)); break;
            case SHL_N      : result = op_shl(eval(node->left, table), eval(node->right, table)); break;
            case ASR_N      : result = op_asr(eval(node->left, table), eval(node->right, table)); break;
            case SHR_N      : result = op_shr(eval(node->left, table), eval(node->right, table)); break;
            case POW_N      : result = op_pow(eval(node->left, table), eval(node->right, table)); break;

            case '?'        : result = op_cond(node->left, node->right, node->righter, table); break;
            case ';'        : freevalue(eval(node->left, table)); result = eval(node->right, table); break;
            default         : LogicError(&node->loc, "Invalid node type: %s", nodetype(node)); break;
        }
    }

    if(mytable) freetable(table);

    return result;
}
