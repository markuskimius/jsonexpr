#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "je_builtin.h"
#include "je_error.h"
#include "je_eval.h"
#include "je_func.h"
#include "je_map.h"
#include "je_node.h"
#include "je_oper.h"
#include "je_symtbl.h"
#include "je_val.h"
#include "je_vec.h"


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static JE_VEC* newlist(JE_NODE* node, JE_SYMTBL* table, JE_VEC* list) {
    if(!list) list = je_newvec();

    if(node) {
        switch(node->type) {
            case ',':
                if(node->left) newlist(node->left, table, list);
                if(node->right) newlist(node->right, table, list);
                break;

            default:
                je_vecpush(list, je_eval(node, table));
                break;
        }
    }

    return list;
}


static JE_MAP* newpair(JE_NODE* node, JE_SYMTBL* table, JE_MAP* list) {
    switch(node->type) {
        case ':': {
            JE_VAL* left = je_eval(node->left, table);
            JE_VAL* right = je_eval(node->right, table);

            if(left->type == JE_STRING_V) JE_MapSet(list, JE_ValToCstr(left), right);
            else {
                JE_ValDelete(right);
                JeRuntimeError(&node->left->loc, "STRING expected, got %s", JE_ValGetType(left));
            }

            JE_ValDelete(left);
            /* Do not free right (it's in the map) */

            break;
        }

        default:
            JeRuntimeError(&node->loc, "PAIR expected, got %s", je_nodetype(node));
            break;
    }

    return list;
}


static JE_MAP* newpairlist(JE_NODE* node, JE_SYMTBL* table, JE_MAP* list) {
    if(!list) list = JE_MapNew();

    if(node) {
        switch(node->type) {
            case ',':
                if(node->left) newpair(node->left, table, list);
                if(node->right) newpairlist(node->right, table, list);
                break;

            default:
                JeRuntimeError(&node->loc, "PAIRLIST expected, got %c", je_nodetype(node));
                break;
        }
    }

    return list;
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_VAL* je_eval(JE_NODE* node, JE_SYMTBL* table) {
    JE_VAL* result = NULL;
    int mytable = 0;

    /* Create a symbol table if none provided */
    if(!table) {
        table = je_newtable(NULL);
        mytable = 1;
    }

    if(node) {
        switch(node->type) {
            case JE_NULL_N     : result = JE_ValNewFromNull(); break;
            case JE_BOOL_N     : result = JE_ValNewFromBool(node->value.i); break;
            case JE_INT_N      : result = JE_ValNewFromInt(node->value.i); break;
            case JE_FLOAT_N    : result = JE_ValNewFromFloat(node->value.f); break;
            case JE_STRING_N   : result = JE_ValNewFromCstr(node->value.s); break;
            case JE_ARRAY_N    : result = JE_ValNewFromVec(newlist(node->left, table, NULL)); break;
            case JE_OBJECT_N   : result = JE_ValNewFromMap(newpairlist(node->left, table, NULL)); break;

            case JE_CALL_N     : result = je_opexec("()", table, node->left, node->right, NULL); break;
            case JE_IDENT_N    : result = je_opexec("*x", table, node, NULL, NULL); break;
            case '['           : result = je_opexec("[]", table, node->left, node->right, NULL); break;
            case '.'           : result = je_opexec(".", table, node->left, node->right, NULL); break;

            case '~'           : result = je_opexec("~", table, node->left, NULL, NULL); break;
            case '!'           : result = je_opexec("!", table, node->left, NULL, NULL); break;
            case JE_UPLUS_N    : result = je_opexec("+x", table, node->left, NULL, NULL); break;
            case JE_UMINUS_N   : result = je_opexec("-x", table, node->left, NULL, NULL); break;

            case '*'           : result = je_opexec("*", table, node->left, node->right, NULL); break;
            case '/'           : result = je_opexec("/", table, node->left, node->right, NULL); break;
            case '%'           : result = je_opexec("%", table, node->left, node->right, NULL); break;
            case '+'           : result = je_opexec("+", table, node->left, node->right, NULL); break;
            case '-'           : result = je_opexec("-", table, node->left, node->right, NULL); break;
            case '|'           : result = je_opexec("|", table, node->left, node->right, NULL); break;
            case '&'           : result = je_opexec("&", table, node->left, node->right, NULL); break;
            case '^'           : result = je_opexec("^", table, node->left, node->right, NULL); break;
            case JE_LT_N       : result = je_opexec("<", table, node->left, node->right, NULL); break;
            case JE_GT_N       : result = je_opexec(">", table, node->left, node->right, NULL); break;
            case JE_POW_N      : result = je_opexec("**", table, node->left, node->right, NULL); break;
            case JE_EQ_N       : result = je_opexec("==", table, node->left, node->right, NULL); break;
            case JE_NE_N       : result = je_opexec("!=", table, node->left, node->right, NULL); break;
            case JE_LE_N       : result = je_opexec("<=", table, node->left, node->right, NULL); break;
            case JE_GE_N       : result = je_opexec(">=", table, node->left, node->right, NULL); break;
            case JE_OR_N       : result = je_opexec("||", table, node->left, node->right, NULL); break;
            case JE_AND_N      : result = je_opexec("&&", table, node->left, node->right, NULL); break;
            case JE_SHL_N      : result = je_opexec("<<", table, node->left, node->right, NULL); break;
            case JE_ASR_N      : result = je_opexec(">>", table, node->left, node->right, NULL); break;
            case JE_SHR_N      : result = je_opexec(">>>", table, node->left, node->right, NULL); break;

            case JE_PREINC_N   : result = je_opexec("++x", table, node->left, NULL, NULL); break;
            case JE_PREDEC_N   : result = je_opexec("--x", table, node->left, NULL, NULL); break;
            case JE_POSTINC_N  : result = je_opexec("x++", table, node->left, NULL, NULL); break;
            case JE_POSTDEC_N  : result = je_opexec("x--", table, node->left, NULL, NULL); break;

            case '='           : result = je_opexec("=", table, node->left, node->right, NULL); break;
            case JE_PLEQ_N     : result = je_opexec("+=", table, node->left, node->right, NULL); break;
            case JE_MIEQ_N     : result = je_opexec("-=", table, node->left, node->right, NULL); break;
            case JE_TIEQ_N     : result = je_opexec("*=", table, node->left, node->right, NULL); break;
            case JE_DIEQ_N     : result = je_opexec("/=", table, node->left, node->right, NULL); break;
            case JE_MOEQ_N     : result = je_opexec("%=", table, node->left, node->right, NULL); break;
            case JE_ANDEQ_N    : result = je_opexec("&=", table, node->left, node->right, NULL); break;
            case JE_XOREQ_N    : result = je_opexec("^=", table, node->left, node->right, NULL); break;
            case JE_OREQ_N     : result = je_opexec("|=", table, node->left, node->right, NULL); break;
            case JE_POWEQ_N    : result = je_opexec("**=", table, node->left, node->right, NULL); break;
            case JE_SHLEQ_N    : result = je_opexec("<<=", table, node->left, node->right, NULL); break;
            case JE_ASREQ_N    : result = je_opexec(">>=", table, node->left, node->right, NULL); break;
            case JE_SHREQ_N    : result = je_opexec(">>>=", table, node->left, node->right, NULL); break;

            case ';'           : result = je_opexec(";", table, node->left, node->right, NULL); break;
            case '?'           : result = je_opexec("?:", table, node->left, node->right, node->righter); break;
            default            : JeLogicError(&node->loc, "Invalid node type: %s", je_nodetype(node)); break;
        }
    }

    if(mytable) je_freetable(table);

    return result;
}
