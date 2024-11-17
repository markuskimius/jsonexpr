#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "je_builtin.h"
#include "je_error.h"
#include "je_eval.h"
#include "je_func.h"
#include "je_map.h"
#include "je_val.h"
#include "je_vec.h"
#include "je_node.h"
#include "je_symtbl.h"
#include "je_util.h"
#include "je.h"


/* ***************************************************************************
* TYPES
*/

typedef struct JE_USERFN {
    JE_NODE* body;
} JE_USERFN;


/* ***************************************************************************
* GLOBALS
*/

static JE_MAP* BINFNS = NULL;
static JE_MAP* BINOPS = NULL;
static size_t ncustfunc = 0;


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static void addfn(JE_MAP* map, const char* key, const char* sig, JE_BINARY_FN fn) {
    je_mapset(map, key, je_funcval(je_newfunc(fn, key, sig)));
}


static JE_VAL* tablemod(JE_SYMTBL* table, JE_NODE* node, int create) {
    JE_VAL* result = NULL;

    switch(node->type) {
        case JE_IDENT_N: {
            result = je_tableget(table, node->value.s);

            /* Create if it doesn't exist? */
            if(!result && create) {
                result = je_nullval();
                je_tableset(table, node->value.s, result, 0);
            }

            break;
        }

        case '[': {
            JE_VAL* left = je_eval(node->left, table);
            JE_VAL* right = je_eval(node->right, table);

            if(left->type == JE_ARRAY_V && right->type == JE_INT_V) {
                /* Index to 1 element past last -> push a new element if "create" is requested */
                if(create && left->value.v->length == right->value.i) {
                    je_vecpush(left->value.v, je_nullval());
                }

                result = je_vecget(left->value.v, right->value.i);
                if(!result) JeRuntimeError(&node->loc, "Invalid index, max %zu, got %lld", left->value.v->length-1, right->value.i);

                /*
                * `result` is an item in `left`, but `left` will disappear upon
                * call to je_freeval(left) later if the # of references to it
                * will become zero.  So we duplicate `result` here if the
                * number of rerences to left is 1 or less.
                */
                if(result && left->value.v->count <= 1) {
                    result = je_dupval(result);
                }
            }
            else if(left->type == JE_OBJECT_V && right->type == JE_STRING_V) {
                result = je_mapget(left->value.m, right->value.s);

                /* Create if it doesn't exist */
                if(!result && create) {
                    result = je_nullval();
                    je_mapset(left->value.m, right->value.s, result);
                }

                if(!result) JeRuntimeError(&node->loc, "Invalid key, %s", je_valqstr(right));

                /* See above */
                if(result && left->value.m->count <= 1) {
                    result = je_dupval(result);
                }
            }
            else if(left->type == JE_ARRAY_V ) JeRuntimeError(&node->loc, "ARRAY index must be INTEGER but got %s", je_valtype(right));
            else if(left->type == JE_OBJECT_V) JeRuntimeError(&node->loc, "OBJECT key must be STRING but got %s", je_valtype(right));
            else                               JeRuntimeError(&node->loc, "ARRAY or OBJECT expected before '[' but got %s", je_valtype(left));

            je_freeval(left);
            je_freeval(right);
            break;
        }

        case '.': {
            JE_VAL* left = je_eval(node->left, table);
            JE_NODE* right = node->right;

            if(left->type == JE_OBJECT_V && right->type == JE_IDENT_N) {
                result = je_mapget(left->value.m, right->value.s);

                /* Create if it doesn't exist */
                if(!result && create) {
                    result = je_nullval();
                    je_mapset(left->value.m, right->value.s, result);
                }

                if(!result) JeRuntimeError(&node->loc, "Invalid key, %s", right->value.s);

                /* See above */
                if(result && left->value.m->count <= 1) {
                    result = je_dupval(result);
                }
            }
            else if(left->type == JE_OBJECT_V) JeRuntimeError(&node->loc, "IDENTIFIER expected after '.' but got %s", je_nodetype(right));
            else                               JeRuntimeError(&node->loc, "OBJECT expected before '.' but got %s", je_valtype(left));

            je_freeval(left);
            break;
        }

        default:
            JeParseError(&node->loc, "Invalid node type: %s", je_nodetype(node));
            break;
    }

    if(!result) {
        JeRuntimeError(&node->loc, "Undefined symbol");
    }

    return result;
}


static void unfold(JE_VEC* vec, JE_NODE* node, JE_SYMTBL* table) {
    switch(node->type) {
        case ',':
            if(node->left) unfold(vec, node->left, table);
            if(node->right) unfold(vec, node->right, table);
            break;

        default:
            je_vecpush(vec, je_nodeval(node));
            break;
    }
}


/* ***************************************************************************
* BUILT-IN FUNCTIONS
*/

static JE_VAL* CEIL(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* value = je_vecget(args, 0);
    JE_VAL* result = NULL;

    if(value->type == JE_FLOAT_V) result = je_intval(ceil(value->value.f));
    else                          result = je_intval(value->value.i);

    return result;
}


static JE_VAL* EVAL(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* value = je_vecget(args, 0);
    JE_NODE* ast = je_parse(je_valstr(value));
    JE_VAL* result = je_eval(ast, table);

    je_freenode(ast);

    return result;
}


static JE_VAL* FLOOR(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* value = je_vecget(args, 0);
    JE_VAL* result = NULL;

    if(value->type == JE_FLOAT_V) result = je_intval(floor(value->value.f));
    else                          result = je_intval(value->value.i);

    return result;
}


static JE_VAL* FOR(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* last = je_eval(args->item[0]->value.n, table);
    JE_VAL* result = NULL;

    while(1) {
        je_freeval(last);
        last = je_eval(args->item[1]->value.n, table);
        if(!je_valtrue(last)) break;

        if(result) je_freeval(result);
        result = je_eval(args->item[3]->value.n, table);

        je_freeval(last);
        last = je_eval(args->item[2]->value.n, table);
    }

    je_freeval(last);

    return result ? result : je_nullval();
}


static JE_VAL* FOREACH(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_NODE* var = je_vecget(args, 0)->value.n;
    JE_VAL* iter = je_vecget(args, 1);
    JE_NODE* expr = je_vecget(args, 2)->value.n;
    JE_VAL* result = NULL;

    /* Validate */
    if(var->type != JE_IDENT_N) JeRuntimeError(&var->loc, "Identifier expected, got %s", je_nodetype(var));

    switch(iter->type) {
        case JE_ARRAY_V: {
            const char* name = var->value.s;
            JE_VEC* vec = iter->value.v;

            for(size_t i=0; i<vec->length; i++) {
                JE_VAL* item = je_vecget(vec, i);

                je_tableset(table, name, je_dupval(item), 1);

                if(result) je_freeval(result);
                result = je_eval(expr, table);
            }

            break;
        }

        case JE_OBJECT_V: {
            const char* name = var->value.s;
            JE_MAP* map = iter->value.m;

            while((map = je_mapnext(map))) {
                JE_VAL* key = je_strval(je_mapkey(map));
                JE_VAL* value = je_mapval(map);
                JE_VEC* pair = je_newvec();

                je_vecpush(pair, key);
                je_vecpush(pair, je_dupval(value));
                je_tableset(table, name, je_arrval(pair), 1);

                if(result) je_freeval(result);
                result = je_eval(expr, table);
            }

            break;
        }

        default:
            JeRuntimeError(&var->loc, "Array or object expected, got %s", je_valtype(iter));
    }

    return result ? result : je_nullval();
}


static JE_VAL* ROUND(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* value = je_vecget(args, 0);
    JE_VAL* result = NULL;

    if(value->type == JE_FLOAT_V) result = je_intval(round(value->value.f));
    else                          result = je_intval(value->value.i);

    return result;
}


static JE_VAL* FUNCTION(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* spec = args->item[0];
    char* sig = je_valstr(spec);
    int isok = 1;

    if(spec->type == JE_STRING_V) {
        int nstars = 0;

        /* Validate the signature argument */
        for(size_t i=0; i<strlen(sig); i++) {
            /* Validate signature */
            if(!strchr("BIDSAOF#@?.*", sig[i])) {
                JeRuntimeError(loc, "Invalid function argument signature: %c", sig[i]);
                isok = 0;
                break;
            }

            /* Signature can only end with a star */
            if(nstars && sig[i] != '*') {
                JeRuntimeError(loc, "'*' must terminate function argument signature");
                isok = 0;
                break;
            }

            /* Maximum # of stars is 2 */
            if(sig[i] == '*' && nstars == 2) {
                JeRuntimeError(loc, "Too many '*', maximum is 2");
                isok = 0;
                break;
            }

            if(sig[i] == '*') nstars++;
        }
    }
    else {
        JeRuntimeError(loc, "Invalid argument type, expected string but got %c", spec->type);
        isok = 0;
    }

    if(isok) {
        char name[64];

        snprintf(name, sizeof(name), "%zd", ++ncustfunc);

        return je_funcval(je_newcustfunc(args->item[1]->value.n, name, sig, table));
    }

    return NULL;
}


static JE_VAL* IF(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* result = NULL;

    for(size_t i=0; i<(args->length & ~1UL); i+=2) {
        JE_VAL* cond = je_eval(args->item[i]->value.n, table);

        if(je_valtrue(cond)) {
            result = je_eval(args->item[i+1]->value.n, table);
            break;
        }
    }

    if(!result && (args->length % 2)) {
        result = je_eval(args->item[args->length-1]->value.n, table);
    }

    return result ? result : je_nullval();
}


static JE_VAL* LEN(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* result = NULL;
    JE_VAL* val = args->item[0];

    switch(val->type) {
        case JE_STRING_V   : result = je_intval(strlen(je_valstr(val))); break;
        case JE_ARRAY_V    : result = je_intval(val->value.v->length); break;
        case JE_OBJECT_V   : result = je_intval(val->value.m->length); break;
        default            : JeRuntimeError(loc, "Type has no length: '%c' (%d)", val->type, val->type); break;
    }

    return result;
}


static JE_VAL* PRINT(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    int64_t i = 0;

    for(i=0; i<args->length; i++) {
        if(i > 0) printf(" ");
        printf("%s", je_valstr(args->item[i]));
    }

    printf("\n");

    return je_intval(i);
}


static JE_VAL* SQRT(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* result = NULL;
    JE_VAL* val = args->item[0];

    switch(val->type) {
        case JE_INT_V      : result = je_dblval(sqrt(val->value.i)); break;
        case JE_FLOAT_V    : result = je_dblval(sqrt(val->value.f)); break;
        default            : JeRuntimeError(loc, "Invalid argument to SQRT(): '%c' (%d)", val->type, val->type); break;
    }

    return result ? result : je_nullval();
}


static JE_VAL* WHILE(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* last = je_eval(args->item[0]->value.n, table);
    JE_VAL* result = NULL;

    while(je_valtrue(last)) {
        if(result) je_freeval(result);
        result = je_eval(args->item[1]->value.n, table);

        je_freeval(last);
        last = je_eval(args->item[0]->value.n, table);
    }

    je_freeval(last);

    return result ? result : je_nullval();
}


/* ***************************************************************************
* BUILT-IN OPERATIONS
*/

static JE_VAL* OP_INC(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = je_vecget(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* result = NULL;

    assert(ref->type == JE_NODE_V);
    left = tablemod(table, ref->value.n, 0);

    if(left) {
        switch(left->type) {
            case JE_INT_V   : result = je_intval(left->value.i += 1); break;
            case JE_FLOAT_V : result = je_dblval(left->value.f += 1.0); break;
            default         : /* noop */ break;
        }
    }

    return result ? result : je_nullval();
}


static JE_VAL* OP_DEC(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = je_vecget(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* result = NULL;

    assert(ref->type == JE_NODE_V);
    left = tablemod(table, ref->value.n, 0);

    if(left) {
        switch(left->type) {
            case JE_INT_V   : result = je_intval(left->value.i -= 1); break;
            case JE_FLOAT_V : result = je_dblval(left->value.f -= 1.0); break;
            default         : /* noop */ break;
        }
    }

    return result ? result : je_nullval();
}


static JE_VAL* OP_INCPOST(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = je_vecget(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* result = NULL;

    assert(ref->type == JE_NODE_V);
    left = tablemod(table, ref->value.n, 0);

    if(left) {
        switch(left->type) {
            case JE_INT_V   : result = je_intval(left->value.i); left->value.i += 1; break;
            case JE_FLOAT_V : result = je_dblval(left->value.f); left->value.f += 1.0; break;
            default         : /* noop */ break;
        }
    }

    return result ? result : je_nullval();
}


static JE_VAL* OP_DECPOST(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = je_vecget(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* result = NULL;

    assert(ref->type == JE_NODE_V);
    left = tablemod(table, ref->value.n, 0);

    if(left) {
        switch(left->type) {
            case JE_INT_V   : result = je_intval(left->value.i); left->value.i -= 1; break;
            case JE_FLOAT_V : result = je_dblval(left->value.f); left->value.f -= 1.0; break;
            default         : /* noop */ break;
        }
    }

    return result ? result : je_nullval();
}


static JE_VAL* OP_SET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = je_vecget(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = je_vecget(args, 1);

    assert(ref->type == JE_NODE_V);

    left = tablemod(table, ref->value.n, 1);
    if(left) je_swapval(left, right);

    return left ? je_dupval(left) : je_nullval();
}


static JE_VAL* OP_ADDSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = je_vecget(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = je_vecget(args, 1);

    assert(ref->type == JE_NODE_V);
    left = tablemod(table, ref->value.n, 1);

    if(left) {
        if     (left->type == JE_INT_V    && right->type == JE_INT_V   ) left->value.i += right->value.i;
        else if(left->type == JE_INT_V    && right->type == JE_FLOAT_V ) { right->value.f += left->value.i; je_swapval(left, right); }
        else if(left->type == JE_FLOAT_V  && right->type == JE_INT_V   ) left->value.f += right->value.i;
        else if(left->type == JE_FLOAT_V  && right->type == JE_FLOAT_V ) left->value.f += right->value.f;
        else if(left->type == JE_STRING_V                              ) left->value.s = je_astrcat(left->value.s, je_valstr(right));
        else if(                             right->type == JE_STRING_V) {
            JE_VAL* tmp = je_strval("");

            tmp->value.s = je_casprintf(tmp->value.s, "%s%s", je_valstr(left), right->value.s);
            je_swapval(tmp, left);
            je_freeval(tmp);
        }
        else left = NULL;
    }

    return left ? je_dupval(left) : je_nullval();
}


static JE_VAL* OP_SUBSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = je_vecget(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = je_vecget(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);
    left = tablemod(table, ref->value.n, 1);

    if(left) {
        if     (left->type == JE_INT_V   && right->type == JE_INT_V  ) left->value.i -= right->value.i;
        else if(left->type == JE_INT_V   && right->type == JE_FLOAT_V) { right->value.f = left->value.i-right->value.f; je_swapval(left, right); }
        else if(left->type == JE_FLOAT_V && right->type == JE_INT_V  ) left->value.f -= right->value.i;
        else if(left->type == JE_FLOAT_V && right->type == JE_FLOAT_V) left->value.f -= right->value.f;
        else left = NULL;
    }

    return left ? je_dupval(left) : je_nullval();
}


static JE_VAL* OP_MULSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = je_vecget(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = je_vecget(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);
    left = tablemod(table, ref->value.n, 1);

    if(left) {
        if     (left->type == JE_INT_V   && right->type == JE_INT_V  ) left->value.i *= right->value.i;
        else if(left->type == JE_INT_V   && right->type == JE_FLOAT_V) { right->value.f = left->value.i*right->value.f; je_swapval(left, right); }
        else if(left->type == JE_FLOAT_V && right->type == JE_INT_V  ) left->value.f *= right->value.i;
        else if(left->type == JE_FLOAT_V && right->type == JE_FLOAT_V) left->value.f *= right->value.f;
        else left = NULL;
    }

    return left ? je_dupval(left) : je_nullval();
}


static JE_VAL* OP_DIVSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = je_vecget(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = je_vecget(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);
    left = tablemod(table, ref->value.n, 1);

    if(left) {
        if     (left->type == JE_INT_V   && right->type == JE_INT_V  ) left->value.i /= right->value.i;
        else if(left->type == JE_INT_V   && right->type == JE_FLOAT_V) { right->value.f = left->value.i/right->value.f; je_swapval(left, right); }
        else if(left->type == JE_FLOAT_V && right->type == JE_INT_V  ) left->value.f /= right->value.i;
        else if(left->type == JE_FLOAT_V && right->type == JE_FLOAT_V) left->value.f /= right->value.f;
        else left = NULL;
    }

    return left ? je_dupval(left) : je_nullval();
}


static JE_VAL* OP_MODSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = je_vecget(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = je_vecget(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);
    left = tablemod(table, ref->value.n, 1);

    if(left) {
        if(left->type == JE_INT_V && right->type == JE_INT_V) left->value.i %= right->value.i;
        else if(left->type == JE_INT_V || left->type == JE_FLOAT_V) {
            double a = left->type==JE_INT_V ? left->value.i : left->value.f;
            double b = right->type==JE_INT_V ? right->value.i : right->value.f;
            JE_VAL* tmp = je_dblval(a - floor(a / b)*b);

            je_swapval(tmp, left);
            je_freeval(tmp);
        }
        else left = NULL;
    }

    return left ? je_dupval(left) : je_nullval();
}


static JE_VAL* OP_POWSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = je_vecget(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = je_vecget(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);
    left = tablemod(table, ref->value.n, 1);

    if(left) {
        if(left->type == JE_INT_V || left->type == JE_FLOAT_V) {
            double a = left->type==JE_INT_V ? left->value.i : left->value.f;
            double b = right->type==JE_INT_V ? right->value.i : right->value.f;
            JE_VAL* tmp = je_dblval(pow(a, b));

            je_swapval(tmp, left);
            je_freeval(tmp);
        }
        else left = NULL;
    }

    return left ? je_dupval(left) : je_nullval();
}

static JE_VAL* OP_ORSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = je_vecget(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = je_vecget(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V);
    left = tablemod(table, ref->value.n, 1);

    if(left) {
        if(left->type == JE_INT_V) left->value.i |= right->value.i;
        else left = NULL;
    }

    return left ? je_dupval(left) : je_nullval();
}


static JE_VAL* OP_ANDSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = je_vecget(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = je_vecget(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V);
    left = tablemod(table, ref->value.n, 1);

    if(left) {
        if(left->type == JE_INT_V) left->value.i &= right->value.i;
        else left = NULL;
    }

    return left ? je_dupval(left) : je_nullval();
}


static JE_VAL* OP_XORSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = je_vecget(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = je_vecget(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V);
    left = tablemod(table, ref->value.n, 1);

    if(left) {
        if(left->type == JE_INT_V) left->value.i ^= right->value.i;
        else left = NULL;
    }

    return left ? je_dupval(left) : je_nullval();
}


static JE_VAL* OP_SHLSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = je_vecget(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = je_vecget(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V);
    left = tablemod(table, ref->value.n, 1);

    if(left) {
        if(left->type == JE_INT_V) {
            int64_t a = left->value.i;
            int64_t b = right->value.i;

            if     (b <-63 && a < 0) left->value.i = -1;
            else if(b <-63         ) left->value.i =  0;
            else if(b <  0 && a < 0) left->value.i = ~(~a >> -b);
            else if(b <  0         ) left->value.i >>= -b;
            else if(b == 0         ) /* no op */ ;
            else if(b < 63         ) left->value.i <<= b;
            else                     left->value.i = 0;
        }
        else left = NULL;
    }

    return left ? je_dupval(left) : je_nullval();
}


static JE_VAL* OP_ASRSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = je_vecget(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = je_vecget(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V);
    left = tablemod(table, ref->value.n, 1);

    if(left) {
        if(left->type == JE_INT_V) {
            int64_t a = left->value.i;
            int64_t b = right->value.i;

            if     (b <-63         ) left->value.i = 0;
            else if(b <  0         ) left->value.i <<= -b;
            else if(b == 0         ) /* no op */ ;
            else if(b < 63 && a < 0) left->value.i = ~(~a >> b);
            else if(b < 63         ) left->value.i >>= b;
            else if(          a < 0) left->value.i = -1;
            else                     left->value.i = 0;
        }
        else left = NULL;
    }

    return left ? je_dupval(left) : je_nullval();
}


static JE_VAL* OP_SHRSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = je_vecget(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = je_vecget(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V);
    left = tablemod(table, ref->value.n, 1);

    if(left) {
        if(left->type == JE_INT_V) {
            uint64_t a = left->value.i;
            int64_t b = right->value.i;

            if     (b <-63) left->value.i = 0;
            else if(b <  0) left->value.i <<= -b;
            else if(b == 0) /* no op */ ;
            else if(b < 63) left->value.i = (a >> b);
            else            left->value.i = 0;
        }
        else left = NULL;
    }

    return left ? je_dupval(left) : je_nullval();
}


static JE_VAL* OP_DEREF(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* symnode = je_vecget(args, 0);
    JE_VAL* result = NULL;

    assert(symnode->type == JE_NODE_V);

    result = tablemod(table, symnode->value.n, 0);

    return result ? je_dupval(result) : je_nullval();
}


static JE_VAL* OP_INDEX(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);
    JE_VAL* result = NULL;

    if(left->type == JE_ARRAY_V && right->type == JE_INT_V) {
        result = je_vecget(left->value.v, right->value.i);
        if(!result) JeRuntimeError(loc, "Invalid index, max %zd, got %lld", left->value.v->length-1, right->value.i);
    }
    else if(left->type == JE_OBJECT_V && right->type == JE_STRING_V) {
        result = je_mapget(left->value.m, right->value.s);
        if(!result) JeRuntimeError(loc, "Invalid key, %s", je_valqstr(right));
    }
    else if(left->type == JE_ARRAY_V ) JeRuntimeError(loc, "ARRAY index must be INTEGER but got %s", je_valtype(right));
    else if(left->type == JE_OBJECT_V) JeRuntimeError(loc, "OBJECT key must be STRING but got %s", je_valtype(right));
    else                               JeRuntimeError(loc, "ARRAY or OBJECT expected before '[' but got %s", je_valtype(left));

    return result ? je_dupval(result) : je_nullval();
}


static JE_VAL* OP_MEMBER(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_NODE* right = je_vecget(args, 1)->value.n;
    JE_VAL* result = NULL;

    if     (left->type == JE_OBJECT_V && right->type == JE_IDENT_N) result = je_mapget(left->value.m, right->value.s);
    else if(left->type == JE_OBJECT_V                             ) JeRuntimeError(loc, "IDENTIFIER expected after '.' but got %s", je_nodetype(right));
    else                                                            JeRuntimeError(loc, "OBJECT expected before '.' but got %s", je_valtype(left));

    return result ? je_dupval(result) : je_nullval();
}


static JE_VAL* OP_TERM(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    return je_dupval(je_vecget(args, 1));
}


static JE_VAL* OP_CALL(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);
    JE_VEC* args2 = je_newvec();
    JE_VAL* result = NULL;

    assert(left->type == JE_FUNCTION_V);
    assert(right == NULL || right->type == JE_NODE_V);

    if(right) unfold(args2, right->value.n, table);
    result = je_funcexec(left->value.fn, args2, table, loc);

    je_freevec(args2);

    return result;
}


static JE_VAL* OP_NOT(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);

    return je_boolval(!je_valtrue(left));
}


static JE_VAL* OP_POS(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* result = NULL;

    assert(left->type == JE_INT_V || left->type == JE_FLOAT_V);

    switch(left->type) {
        case JE_INT_V   : result = je_intval(left->value.i); break;
        case JE_FLOAT_V : result = je_dblval(left->value.f); break;
        default         : result = je_nullval(); break;
    }

    return result ? result : je_nullval();
}


static JE_VAL* OP_NEG(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* result = NULL;

    assert(left->type == JE_INT_V || left->type == JE_FLOAT_V);

    switch(left->type) {
        case JE_INT_V   : result = je_intval(-left->value.i); break;
        case JE_FLOAT_V : result = je_dblval(-left->value.f); break;
        default         : result = je_nullval(); break;
    }

    return result ? result : je_nullval();
}


static JE_VAL* OP_INV(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);

    assert(left->type == JE_INT_V);

    return je_intval(~left->value.i);
}


static JE_VAL* OP_MUL(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);
    JE_VAL* result = NULL;

    assert(left->type == JE_INT_V || left->type == JE_FLOAT_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);

    if     (left->type == JE_INT_V   && right->type == JE_INT_V  ) result = je_intval(left->value.i * right->value.i);
    else if(left->type == JE_INT_V   && right->type == JE_FLOAT_V) result = je_dblval(left->value.i * right->value.f);
    else if(left->type == JE_FLOAT_V && right->type == JE_INT_V  ) result = je_dblval(left->value.f * right->value.i);
    else if(left->type == JE_FLOAT_V && right->type == JE_FLOAT_V) result = je_dblval(left->value.f * right->value.f);

    return result ? result : je_nullval();
}


static JE_VAL* OP_DIV(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);
    JE_VAL* result = NULL;

    assert(left->type == JE_INT_V || left->type == JE_FLOAT_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);

    if     (left->type == JE_INT_V   && right->type == JE_INT_V  ) result = je_intval(left->value.i / right->value.i);
    else if(left->type == JE_INT_V   && right->type == JE_FLOAT_V) result = je_dblval(left->value.i / right->value.f);
    else if(left->type == JE_FLOAT_V && right->type == JE_INT_V  ) result = je_dblval(left->value.f / right->value.i);
    else if(left->type == JE_FLOAT_V && right->type == JE_FLOAT_V) result = je_dblval(left->value.f / right->value.f);

    return result ? result : je_nullval();
}


static JE_VAL* OP_MOD(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);
    JE_VAL* result = NULL;

    assert(left->type == JE_INT_V || left->type == JE_FLOAT_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);

    if(left->type == JE_INT_V && right->type == JE_INT_V) result = je_intval(left->value.i % right->value.i);
    else {
        double a = left->type == JE_INT_V ? left->value.i : left->value.f;
        double b = right->type == JE_INT_V ? right->value.i : right->value.f;

        result = je_dblval(a - floor(a / b)*b);
    }

    return result ? result : je_nullval();
}


static JE_VAL* OP_ADD(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);
    JE_VAL* result = NULL;

    if     (left->type == JE_INT_V    && right->type == JE_INT_V   ) result = je_intval(left->value.i + right->value.i);
    else if(left->type == JE_INT_V    && right->type == JE_FLOAT_V ) result = je_dblval(left->value.i + right->value.f);
    else if(left->type == JE_FLOAT_V  && right->type == JE_INT_V   ) result = je_dblval(left->value.f + right->value.i);
    else if(left->type == JE_FLOAT_V  && right->type == JE_FLOAT_V ) result = je_dblval(left->value.f + right->value.f);
    else if(left->type == JE_STRING_V || right->type == JE_STRING_V) {
        char* buf = NULL;

        asprintf(&buf, "%s%s", je_valstr(left), je_valstr(right));
        result = je_strval(buf);
        free(buf);
    }

    return result ? result : je_nullval();
}


static JE_VAL* OP_SUB(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);
    JE_VAL* result = NULL;

    assert(left->type == JE_INT_V || left->type == JE_FLOAT_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);

    if     (left->type == JE_INT_V   && right->type == JE_INT_V  ) result = je_intval(left->value.i - right->value.i);
    else if(left->type == JE_INT_V   && right->type == JE_FLOAT_V) result = je_dblval(left->value.i - right->value.f);
    else if(left->type == JE_FLOAT_V && right->type == JE_INT_V  ) result = je_dblval(left->value.f - right->value.i);
    else if(left->type == JE_FLOAT_V && right->type == JE_FLOAT_V) result = je_dblval(left->value.f - right->value.f);

    return result ? result : je_nullval();
}


static JE_VAL* OP_POW(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);
    JE_VAL* result = NULL;

    assert(left->type == JE_INT_V || left->type == JE_FLOAT_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);

    if     (left->type == JE_INT_V   && right->type == JE_INT_V  ) result = je_dblval(pow(left->value.i, right->value.i));
    else if(left->type == JE_INT_V   && right->type == JE_FLOAT_V) result = je_dblval(pow(left->value.i, right->value.f));
    else if(left->type == JE_FLOAT_V && right->type == JE_INT_V  ) result = je_dblval(pow(left->value.f, right->value.i));
    else if(left->type == JE_FLOAT_V && right->type == JE_FLOAT_V) result = je_dblval(pow(left->value.f, right->value.f));

    return result ? result : je_nullval();
}


static JE_VAL* OP_OR(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);

    assert(left->type == JE_INT_V);
    assert(right->type == JE_INT_V);

    return je_intval(left->value.i | right->value.i);
}


static JE_VAL* OP_AND(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);

    assert(left->type == JE_INT_V);
    assert(right->type == JE_INT_V);

    return je_intval(left->value.i & right->value.i);
}


static JE_VAL* OP_XOR(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);

    assert(left->type == JE_INT_V);
    assert(right->type == JE_INT_V);

    return je_intval(left->value.i ^ right->value.i);
}


static JE_VAL* OP_LT(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);

    return je_boolval(je_valcmp(left, right) < 0);
}


static JE_VAL* OP_GT(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);

    return je_boolval(je_valcmp(left, right) > 0);
}


static JE_VAL* OP_EQ(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);

    return je_boolval(je_valcmp(left, right) == 0);
}


static JE_VAL* OP_NE(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);

    return je_boolval(je_valcmp(left, right) != 0);
}


static JE_VAL* OP_LE(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);

    return je_boolval(je_valcmp(left, right) <= 0);
}


static JE_VAL* OP_GE(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);

    return je_boolval(je_valcmp(left, right) >= 0);
}


static JE_VAL* OP_LOR(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);
    JE_VAL* result = NULL;

    assert(left->type == JE_NODE_V);
    assert(right->type == JE_NODE_V);

    result = je_eval(left->value.n, table);

    if(!je_valtrue(result)) {
        je_freeval(result);
        result = je_eval(right->value.n, table);
    }

    return result;
}


static JE_VAL* OP_LAND(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);
    JE_VAL* result = NULL;

    assert(left->type == JE_NODE_V);
    assert(right->type == JE_NODE_V);

    result = je_eval(left->value.n, table);

    if(je_valtrue(result)) {
        je_freeval(result);
        result = je_eval(right->value.n, table);
    }

    return result;
}


static JE_VAL* OP_SHL(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);
    JE_VAL* result = NULL;
    int64_t a = left->value.i;
    int64_t b = right->value.i;

    assert(left->type == JE_INT_V);
    assert(right->type == JE_INT_V);

    if     (b <-63 && a < 0) result = je_intval(-1);
    else if(b <-63         ) result = je_intval(0);
    else if(b <  0 && a < 0) result = je_intval(~(~a >> -b));
    else if(b <  0         ) result = je_intval(a >> -b);
    else if(b == 0         ) result = je_intval(a);
    else if(b < 63         ) result = je_intval(a << b);
    else                     result = je_intval(0);

    return result ? result : je_nullval();
}


static JE_VAL* OP_ASR(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);
    JE_VAL* result = NULL;
    int64_t a = left->value.i;
    int64_t b = right->value.i;

    assert(left->type == JE_INT_V);
    assert(right->type == JE_INT_V);

    if     (b <-63         ) result = je_intval(0);
    else if(b <  0         ) result = je_intval(a << -b);
    else if(b == 0         ) result = je_intval(a);
    else if(b < 63 && a < 0) result = je_intval(~(~a >> b));
    else if(b < 63         ) result = je_intval(a >> b);
    else if(          a < 0) result = je_intval(-1);
    else                     result = je_intval(0);

    return result ? result : je_nullval();
}


static JE_VAL* OP_SHR(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);
    JE_VAL* result = NULL;
    uint64_t a = left->value.i;
    int64_t b = right->value.i;

    assert(left->type == JE_INT_V);
    assert(right->type == JE_INT_V);

    if     (b <-63) result = je_intval(0);
    else if(b <  0) result = je_intval(a << -b);
    else if(b == 0) result = je_intval(a);
    else if(b < 63) result = je_intval(a >> b);
    else            result = je_intval(0);

    return result ? result : je_nullval();
}


static JE_VAL* OP_COND(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = je_vecget(args, 0);
    JE_VAL* right = je_vecget(args, 1);
    JE_VAL* righter = je_vecget(args, 2);
    JE_VAL* result = NULL;

    assert(left->type == JE_NODE_V);
    assert(right->type == JE_NODE_V);
    assert(righter->type == JE_NODE_V);

    result = je_eval(left->value.n, table);

    if(je_valtrue(result)) {
        je_freeval(result);
        result = je_eval(right->value.n, table);
    }
    else {
        je_freeval(result);
        result = je_eval(righter->value.n, table);
    }

    return result;
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_MAP* je_binfns() {
    if(!BINFNS) {
        BINFNS = je_newmap();

        je_mapset(BINFNS, "VERSION", je_strval(JE_VERSION));
        je_mapset(BINFNS, "VERSION_MAJOR", je_intval(JE_VERSION_MAJOR));
        je_mapset(BINFNS, "VERSION_MINOR", je_intval(JE_VERSION_MINOR));
        je_mapset(BINFNS, "VERSION_PATCH", je_intval(JE_VERSION_PATCH));

        addfn(BINFNS, "CEIL"    , "#"   , CEIL    );
        addfn(BINFNS, "EVAL"    , "S"   , EVAL    );
        addfn(BINFNS, "FLOOR"   , "#"   , FLOOR   );
        addfn(BINFNS, "FOR"     , "....", FOR     );
        addfn(BINFNS, "FOREACH" , ".@." , FOREACH );
        addfn(BINFNS, "FUNCTION", "S."  , FUNCTION);
        addfn(BINFNS, "IF"      , ".**" , IF      );
        addfn(BINFNS, "LEN"     , "?"   , LEN     );
        addfn(BINFNS, "PRINT"   , "*"   , PRINT   );
        addfn(BINFNS, "ROUND"   , "#"   , ROUND   );
        addfn(BINFNS, "SQRT"    , "?"   , SQRT    );
        addfn(BINFNS, "WHILE"   , ".."  , WHILE   );
    }

    return BINFNS;
}


JE_MAP* je_binops() {
    if(!BINOPS) {
        BINOPS = je_newmap();

        addfn(BINOPS, "++x" , "."  , OP_INC     );
        addfn(BINOPS, "--x" , "."  , OP_DEC     );
        addfn(BINOPS, "x++" , "."  , OP_INCPOST );
        addfn(BINOPS, "x--" , "."  , OP_DECPOST );

        addfn(BINOPS, "="   , ".?" , OP_SET     );
        addfn(BINOPS, "+="  , ".?" , OP_ADDSET  );
        addfn(BINOPS, "-="  , ".#" , OP_SUBSET  );
        addfn(BINOPS, "*="  , ".#" , OP_MULSET  );
        addfn(BINOPS, "/="  , ".#" , OP_DIVSET  );
        addfn(BINOPS, "%="  , ".#" , OP_MODSET  );
        addfn(BINOPS, "**=" , ".#" , OP_POWSET  );

        addfn(BINOPS, "|="  , ".I" , OP_ORSET   );
        addfn(BINOPS, "&="  , ".I" , OP_ANDSET  );
        addfn(BINOPS, "^="  , ".I" , OP_XORSET  );
        addfn(BINOPS, "<<=" , ".I" , OP_SHLSET  );
        addfn(BINOPS, ">>=" , ".I" , OP_ASRSET  );
        addfn(BINOPS, ">>>=", ".I" , OP_SHRSET  );

        addfn(BINOPS, "*x"  , "."  , OP_DEREF   );
        addfn(BINOPS, "[]"  , "??" , OP_INDEX   );
        addfn(BINOPS, "."   , "?." , OP_MEMBER  );
        addfn(BINOPS, ";"   , "??" , OP_TERM    );
        addfn(BINOPS, "()"  , "F**", OP_CALL    );

        addfn(BINOPS, "!"   , "?"  , OP_NOT     );
        addfn(BINOPS, "~"   , "I"  , OP_INV     );
        addfn(BINOPS, "+x"  , "#"  , OP_POS     );
        addfn(BINOPS, "-x"  , "#"  , OP_NEG     );

        addfn(BINOPS, "*"   , "##" , OP_MUL     );
        addfn(BINOPS, "/"   , "##" , OP_DIV     );
        addfn(BINOPS, "%"   , "##" , OP_MOD     );
        addfn(BINOPS, "+"   , "??" , OP_ADD     );
        addfn(BINOPS, "-"   , "##" , OP_SUB     );
        addfn(BINOPS, "|"   , "II" , OP_OR      );
        addfn(BINOPS, "&"   , "II" , OP_AND     );
        addfn(BINOPS, "^"   , "II" , OP_XOR     );
        addfn(BINOPS, "<"   , "??" , OP_LT      );
        addfn(BINOPS, ">"   , "??" , OP_GT      );
        addfn(BINOPS, "**"  , "##" , OP_POW     );
        addfn(BINOPS, "=="  , "??" , OP_EQ      );
        addfn(BINOPS, "!="  , "??" , OP_NE      );
        addfn(BINOPS, "<="  , "??" , OP_LE      );
        addfn(BINOPS, ">="  , "??" , OP_GE      );
        addfn(BINOPS, "||"  , ".." , OP_LOR     );
        addfn(BINOPS, "&&"  , ".." , OP_LAND    );
        addfn(BINOPS, "<<"  , "II" , OP_SHL     );
        addfn(BINOPS, ">>"  , "II" , OP_ASR     );
        addfn(BINOPS, ">>>" , "II" , OP_SHR     );

        addfn(BINOPS, "?:"  , "...", OP_COND    );
    }

    return BINOPS;
}


JE_VAL* je_opexec(const char* key, JE_SYMTBL* table, JE_NODE* left, JE_NODE* right, JE_NODE* righter) {
    JE_VAL* fnval = je_mapget(je_binops(), key);
    JE_VAL* result = NULL;

    if(fnval && fnval->type == JE_FUNCTION_V) {
        JE_FUNC* fn = fnval->value.fn;
        JE_VEC* args = je_newvec();

        if(left) je_vecpush(args, je_nodeval(left));
        if(right) je_vecpush(args, je_nodeval(right));
        if(righter) je_vecpush(args, je_nodeval(righter));

        result = je_funcexec(fn, args, table, &left->loc);
        je_freevec(args);
    }

    return result ? result : je_nullval();
}
