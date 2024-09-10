#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtin.h"
#include "error.h"
#include "eval.h"
#include "func.h"
#include "map.h"
#include "node.h"
#include "oper.h"
#include "symtbl.h"
#include "val.h"
#include "vec.h"


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
            VAL* right = eval(node->right, table);

            if(left->type == STRING_N) mapset(list, left->value.s, right);
            else {
                freeval(right);
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


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VAL* eval(NODE* node, SYMTBL* table) {
    VAL* result = NULL;
    int mytable = 0;

    /* Create a symbol table if none provided */
    if(!table) {
        table = newtable(NULL);
        mytable = 1;
    }

    if(node) {
        switch(node->type) {
            case NULL_N     : result = nullval(); break;
            case BOOL_N     : result = boolval(node->value.i); break;
            case INT_N      : result = intval(node->value.i); break;
            case FLOAT_N    : result = dblval(node->value.f); break;
            case STRING_N   : result = strval(node->value.s); break;
            case ARRAY_N    : result = arrval(newlist(node->left, table, NULL)); break;
            case OBJECT_N   : result = objval(newpairlist(node->left, table, NULL)); break;

            case SYMBOL_N   : result = opexec("*x", table, node->left, NULL, NULL); break;
            case CALL_N     : result = opexec("()", table, node->left, node->right, NULL); break;

            case '~'        : result = opexec("~", table, node->left, NULL, NULL); break;
            case '!'        : result = opexec("!", table, node->left, NULL, NULL); break;
            case UPLUS_N    : result = opexec("+x", table, node->left, NULL, NULL); break;
            case UMINUS_N   : result = opexec("-x", table, node->left, NULL, NULL); break;

            case '*'        : result = opexec("*", table, node->left, node->right, NULL); break;
            case '/'        : result = opexec("/", table, node->left, node->right, NULL); break;
            case '%'        : result = opexec("%", table, node->left, node->right, NULL); break;
            case '+'        : result = opexec("+", table, node->left, node->right, NULL); break;
            case '-'        : result = opexec("-", table, node->left, node->right, NULL); break;
            case '|'        : result = opexec("|", table, node->left, node->right, NULL); break;
            case '&'        : result = opexec("&", table, node->left, node->right, NULL); break;
            case '^'        : result = opexec("^", table, node->left, node->right, NULL); break;
            case LT_N       : result = opexec("<", table, node->left, node->right, NULL); break;
            case GT_N       : result = opexec(">", table, node->left, node->right, NULL); break;
            case POW_N      : result = opexec("**", table, node->left, node->right, NULL); break;
            case EQ_N       : result = opexec("==", table, node->left, node->right, NULL); break;
            case NE_N       : result = opexec("!=", table, node->left, node->right, NULL); break;
            case LE_N       : result = opexec("<=", table, node->left, node->right, NULL); break;
            case GE_N       : result = opexec(">=", table, node->left, node->right, NULL); break;
            case OR_N       : result = opexec("||", table, node->left, node->right, NULL); break;
            case AND_N      : result = opexec("&&", table, node->left, node->right, NULL); break;
            case SHL_N      : result = opexec("<<", table, node->left, node->right, NULL); break;
            case ASR_N      : result = opexec(">>", table, node->left, node->right, NULL); break;
            case SHR_N      : result = opexec(">>>", table, node->left, node->right, NULL); break;

            case PREINC_N   : result = opexec("++x", table, node->left, NULL, NULL); break;
            case PREDEC_N   : result = opexec("--x", table, node->left, NULL, NULL); break;
            case POSTINC_N  : result = opexec("x++", table, node->left, NULL, NULL); break;
            case POSTDEC_N  : result = opexec("x--", table, node->left, NULL, NULL); break;

            case '='        : result = opexec("=", table, node->left, node->right, NULL); break;
            case PLEQ_N     : result = opexec("+=", table, node->left, node->right, NULL); break;
            case MIEQ_N     : result = opexec("-=", table, node->left, node->right, NULL); break;
            case TIEQ_N     : result = opexec("*=", table, node->left, node->right, NULL); break;
            case DIEQ_N     : result = opexec("/=", table, node->left, node->right, NULL); break;
            case MOEQ_N     : result = opexec("%=", table, node->left, node->right, NULL); break;
            case ANDEQ_N    : result = opexec("&=", table, node->left, node->right, NULL); break;
            case XOREQ_N    : result = opexec("^=", table, node->left, node->right, NULL); break;
            case OREQ_N     : result = opexec("|=", table, node->left, node->right, NULL); break;
            case POWEQ_N    : result = opexec("**=", table, node->left, node->right, NULL); break;
            case SHLEQ_N    : result = opexec("<<=", table, node->left, node->right, NULL); break;
            case ASREQ_N    : result = opexec(">>=", table, node->left, node->right, NULL); break;
            case SHREQ_N    : result = opexec(">>>=", table, node->left, node->right, NULL); break;

            case ';'        : result = opexec(";", table, node->left, node->right, NULL); break;
            case '?'        : result = opexec("?:", table, node->left, node->right, node->righter); break;
            default         : LogicError(&node->loc, "Invalid node type: %s", nodetype(node)); break;
        }
    }

    if(mytable) freetable(table);

    return result;
}
