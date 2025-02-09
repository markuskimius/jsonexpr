#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
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
* PRIVATE VARIABLES
*/

static JE_MAP* _BINFNS = NULL;
static JE_MAP* _BINOPS = NULL;
static size_t _ncustfunc = 0;


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static void _addtomap(JE_MAP* map, const char* key, const char* sig, JE_BINARY_FN fn) {
    JE_MapSet(map, key, JE_ValNewFromFunc(JE_FuncNew(fn, key, sig)));
}

static JE_VAL* _symtblGetByNode(JE_SYMTBL* table, JE_NODE* node, int create) {
    JE_VAL* result = NULL;

    switch(node->type) {
        case JE_IDENT_N: {
            result = JE_SymtblGet(table, node->value.s);

            /* Create if it doesn't exist? */
            if(!result && create) {
                result = JE_ValNewFromNull();
                JE_SymtblSet(table, node->value.s, result, 0);
            }

            break;
        }

        case '[': {
            JE_VAL* left = JE_EvalByNode(node->left, table);
            JE_VAL* right = JE_EvalByNode(node->right, table);

            if(left->type == JE_ARRAY_V && right->type == JE_INT_V) {
                /* Index to 1 element past last -> push a new element if "create" is requested */
                if(create && left->value.v->length == right->value.i) {
                    JE_VecPush(left->value.v, JE_ValNewFromNull());
                }

                result = JE_VecGet(left->value.v, right->value.i);
                if(!result) JeRuntimeError(&node->loc, "Invalid index, max %zu, got %lld", left->value.v->length-1, right->value.i);

                /*
                * `result` is an item in `left`, but `left` will disappear upon
                * call to JE_ValDelete(left) later if the # of references to it
                * will become zero.  So we duplicate `result` here if the
                * number of rerences to left is 1 or less.
                */
                if(result && left->value.v->count <= 1) {
                    result = JE_ValDup(result);
                }
            }
            else if(left->type == JE_OBJECT_V && right->type == JE_STRING_V) {
                result = JE_MapGet(left->value.m, right->value.s);

                /* Create if it doesn't exist */
                if(!result && create) {
                    result = JE_ValNewFromNull();
                    JE_MapSet(left->value.m, right->value.s, result);
                }

                if(!result) JeRuntimeError(&node->loc, "Invalid key, %s", JE_ValToQstr(right));

                /* See above */
                if(result && left->value.m->count <= 1) {
                    result = JE_ValDup(result);
                }
            }
            else if(left->type == JE_ARRAY_V ) JeRuntimeError(&node->loc, "ARRAY index must be INTEGER but got %s", JE_ValGetType(right));
            else if(left->type == JE_OBJECT_V) JeRuntimeError(&node->loc, "OBJECT key must be STRING but got %s", JE_ValGetType(right));
            else                               JeRuntimeError(&node->loc, "ARRAY or OBJECT expected before '[' but got %s", JE_ValGetType(left));

            JE_ValDelete(left);
            JE_ValDelete(right);
            break;
        }

        case '.': {
            JE_VAL* left = JE_EvalByNode(node->left, table);
            JE_NODE* right = node->right;

            if(left->type == JE_OBJECT_V && right->type == JE_IDENT_N) {
                result = JE_MapGet(left->value.m, right->value.s);

                /* Create if it doesn't exist */
                if(!result && create) {
                    result = JE_ValNewFromNull();
                    JE_MapSet(left->value.m, right->value.s, result);
                }

                if(!result) JeRuntimeError(&node->loc, "Invalid key, %s", right->value.s);

                /* See above */
                if(result && left->value.m->count <= 1) {
                    result = JE_ValDup(result);
                }
            }
            else if(left->type == JE_OBJECT_V) JeRuntimeError(&node->loc, "IDENTIFIER expected after '.' but got %s", JE_NodeTypeCstr(right));
            else                               JeRuntimeError(&node->loc, "OBJECT expected before '.' but got %s", JE_ValGetType(left));

            JE_ValDelete(left);
            break;
        }

        default:
            JeParseError(&node->loc, "Invalid node type: %s", JE_NodeTypeCstr(node));
            break;
    }

    if(!result) {
        JeRuntimeError(&node->loc, "Undefined symbol");
    }

    return result;
}

static void _unfold(JE_VEC* vec, JE_NODE* node, JE_SYMTBL* table) {
    switch(node->type) {
        case ',':
            if(node->left) _unfold(vec, node->left, table);
            if(node->right) _unfold(vec, node->right, table);
            break;

        default:
            JE_VecPush(vec, JE_ValNewFromNode(node));
            break;
    }
}


/* ***************************************************************************
* BUILT-IN FUNCTIONS
*/

static JE_VAL* _CEIL(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* value = JE_VecGet(args, 0);
    JE_VAL* result = NULL;

    if(value->type == JE_FLOAT_V) result = JE_ValNewFromInt(ceil(value->value.f));
    else                          result = JE_ValNewFromInt(value->value.i);

    return result;
}

static JE_VAL* _EVAL(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* value = JE_VecGet(args, 0);
    JE_NODE* ast = JE_Parse(JE_ValToCstr(value));
    JE_VAL* result = JE_EvalByNode(ast, table);

    JE_NodeDelete(ast);

    return result;
}

static JE_VAL* _FLOOR(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* value = JE_VecGet(args, 0);
    JE_VAL* result = NULL;

    if(value->type == JE_FLOAT_V) result = JE_ValNewFromInt(floor(value->value.f));
    else                          result = JE_ValNewFromInt(value->value.i);

    return result;
}

static JE_VAL* _FOR(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* last = JE_EvalByNode(args->item[0]->value.n, table);
    JE_VAL* result = NULL;

    while(1) {
        JE_ValDelete(last);
        last = JE_EvalByNode(args->item[1]->value.n, table);
        if(!JE_ValIsTrue(last)) break;

        if(result) JE_ValDelete(result);
        result = JE_EvalByNode(args->item[3]->value.n, table);

        JE_ValDelete(last);
        last = JE_EvalByNode(args->item[2]->value.n, table);
    }

    JE_ValDelete(last);

    return result ? result : JE_ValNewFromNull();
}

static JE_VAL* _FOREACH(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_NODE* var = JE_VecGet(args, 0)->value.n;
    JE_VAL* iter = JE_VecGet(args, 1);
    JE_NODE* expr = JE_VecGet(args, 2)->value.n;
    JE_VAL* result = NULL;

    /* Validate */
    if(var->type != JE_IDENT_N) JeRuntimeError(&var->loc, "Identifier expected, got %s", JE_NodeTypeCstr(var));

    switch(iter->type) {
        case JE_ARRAY_V: {
            const char* name = var->value.s;
            JE_VEC* vec = iter->value.v;

            for(size_t i=0; i<vec->length; i++) {
                JE_VAL* item = JE_VecGet(vec, i);

                JE_SymtblSet(table, name, JE_ValDup(item), 1);

                if(result) JE_ValDelete(result);
                result = JE_EvalByNode(expr, table);
            }

            break;
        }

        case JE_OBJECT_V: {
            const char* name = var->value.s;
            JE_MAP* map = iter->value.m;

            while((map = JE_MapNext(map))) {
                JE_VAL* key = JE_ValNewFromCstr(JE_MapKey(map));
                JE_VAL* value = JE_MapVal(map);
                JE_VEC* pair = JE_VecNew();

                JE_VecPush(pair, key);
                JE_VecPush(pair, JE_ValDup(value));
                JE_SymtblSet(table, name, JE_ValNewFromVec(pair), 1);

                if(result) JE_ValDelete(result);
                result = JE_EvalByNode(expr, table);
            }

            break;
        }

        default:
            JeRuntimeError(&var->loc, "Array or object expected, got %s", JE_ValGetType(iter));
    }

    return result ? result : JE_ValNewFromNull();
}

static JE_VAL* _ROUND(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* value = JE_VecGet(args, 0);
    JE_VAL* result = NULL;

    if(value->type == JE_FLOAT_V) result = JE_ValNewFromInt(round(value->value.f));
    else                          result = JE_ValNewFromInt(value->value.i);

    return result;
}

static JE_VAL* _FUNCTION(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* spec = args->item[0];
    const char* sig = JE_ValToCstr(spec);
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

        snprintf(name, sizeof(name), "%zd", ++_ncustfunc);

        return JE_ValNewFromFunc(JE_FuncNewUser(args->item[1]->value.n, name, sig, table));
    }

    return NULL;
}

static JE_VAL* _IF(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* result = NULL;

    for(size_t i=0; i<(args->length & ~1UL); i+=2) {
        JE_VAL* cond = JE_EvalByNode(args->item[i]->value.n, table);

        if(JE_ValIsTrue(cond)) {
            result = JE_EvalByNode(args->item[i+1]->value.n, table);
            break;
        }
    }

    if(!result && (args->length % 2)) {
        result = JE_EvalByNode(args->item[args->length-1]->value.n, table);
    }

    return result ? result : JE_ValNewFromNull();
}

static JE_VAL* _LEN(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* result = NULL;
    JE_VAL* val = args->item[0];

    switch(val->type) {
        case JE_STRING_V   : result = JE_ValNewFromInt(strlen(JE_ValToCstr(val))); break;
        case JE_ARRAY_V    : result = JE_ValNewFromInt(val->value.v->length); break;
        case JE_OBJECT_V   : result = JE_ValNewFromInt(val->value.m->length); break;
        default            : JeRuntimeError(loc, "Type has no length: '%c' (%d)", val->type, val->type); break;
    }

    return result;
}

static JE_VAL* _PRINT(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    int64_t i = 0;

    for(i=0; i<args->length; i++) {
        if(i > 0) printf(" ");
        printf("%s", JE_ValToCstr(args->item[i]));
    }

    printf("\n");

    return JE_ValNewFromInt(i);
}

static JE_VAL* _SQRT(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* result = NULL;
    JE_VAL* val = args->item[0];

    switch(val->type) {
        case JE_INT_V      : result = JE_ValNewFromFloat(sqrt(val->value.i)); break;
        case JE_FLOAT_V    : result = JE_ValNewFromFloat(sqrt(val->value.f)); break;
        default            : JeRuntimeError(loc, "Invalid argument to SQRT(): '%c' (%d)", val->type, val->type); break;
    }

    return result ? result : JE_ValNewFromNull();
}

static JE_VAL* _WHILE(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* last = JE_EvalByNode(args->item[0]->value.n, table);
    JE_VAL* result = NULL;

    while(JE_ValIsTrue(last)) {
        if(result) JE_ValDelete(result);
        result = JE_EvalByNode(args->item[1]->value.n, table);

        JE_ValDelete(last);
        last = JE_EvalByNode(args->item[0]->value.n, table);
    }

    JE_ValDelete(last);

    return result ? result : JE_ValNewFromNull();
}


/* ***************************************************************************
* BUILT-IN OPERATIONS
*/

static JE_VAL* _OP_INC(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = JE_VecGet(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* result = NULL;

    assert(ref->type == JE_NODE_V);
    left = _symtblGetByNode(table, ref->value.n, 0);

    if(left) {
        switch(left->type) {
            case JE_INT_V   : result = JE_ValNewFromInt(left->value.i += 1); break;
            case JE_FLOAT_V : result = JE_ValNewFromFloat(left->value.f += 1.0); break;
            default         : /* noop */ break;
        }
    }

    return result ? result : JE_ValNewFromNull();
}

static JE_VAL* _OP_DEC(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = JE_VecGet(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* result = NULL;

    assert(ref->type == JE_NODE_V);
    left = _symtblGetByNode(table, ref->value.n, 0);

    if(left) {
        switch(left->type) {
            case JE_INT_V   : result = JE_ValNewFromInt(left->value.i -= 1); break;
            case JE_FLOAT_V : result = JE_ValNewFromFloat(left->value.f -= 1.0); break;
            default         : /* noop */ break;
        }
    }

    return result ? result : JE_ValNewFromNull();
}

static JE_VAL* _OP_INCPOST(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = JE_VecGet(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* result = NULL;

    assert(ref->type == JE_NODE_V);
    left = _symtblGetByNode(table, ref->value.n, 0);

    if(left) {
        switch(left->type) {
            case JE_INT_V   : result = JE_ValNewFromInt(left->value.i); left->value.i += 1; break;
            case JE_FLOAT_V : result = JE_ValNewFromFloat(left->value.f); left->value.f += 1.0; break;
            default         : /* noop */ break;
        }
    }

    return result ? result : JE_ValNewFromNull();
}

static JE_VAL* _OP_DECPOST(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = JE_VecGet(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* result = NULL;

    assert(ref->type == JE_NODE_V);
    left = _symtblGetByNode(table, ref->value.n, 0);

    if(left) {
        switch(left->type) {
            case JE_INT_V   : result = JE_ValNewFromInt(left->value.i); left->value.i -= 1; break;
            case JE_FLOAT_V : result = JE_ValNewFromFloat(left->value.f); left->value.f -= 1.0; break;
            default         : /* noop */ break;
        }
    }

    return result ? result : JE_ValNewFromNull();
}

static JE_VAL* _OP_SET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = JE_VecGet(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = JE_VecGet(args, 1);

    assert(ref->type == JE_NODE_V);

    left = _symtblGetByNode(table, ref->value.n, 1);
    if(left) JE_ValSwap(left, right);

    return left ? JE_ValDup(left) : JE_ValNewFromNull();
}

static JE_VAL* _OP_ADDSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = JE_VecGet(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = JE_VecGet(args, 1);

    assert(ref->type == JE_NODE_V);
    left = _symtblGetByNode(table, ref->value.n, 1);

    if(left) {
        if     (left->type == JE_INT_V    && right->type == JE_INT_V   ) left->value.i += right->value.i;
        else if(left->type == JE_INT_V    && right->type == JE_FLOAT_V ) { right->value.f += left->value.i; JE_ValSwap(left, right); }
        else if(left->type == JE_FLOAT_V  && right->type == JE_INT_V   ) left->value.f += right->value.i;
        else if(left->type == JE_FLOAT_V  && right->type == JE_FLOAT_V ) left->value.f += right->value.f;
        else if(left->type == JE_STRING_V                              ) left->value.s = JE_AstrCat(left->value.s, JE_ValToCstr(right));
        else if(                             right->type == JE_STRING_V) {
            JE_VAL* tmp = JE_ValNewFromCstr("");

            tmp->value.s = JE_AstrCatFormat(tmp->value.s, "%s%s", JE_ValToCstr(left), right->value.s);
            JE_ValSwap(tmp, left);
            JE_ValDelete(tmp);
        }
        else left = NULL;
    }

    return left ? JE_ValDup(left) : JE_ValNewFromNull();
}

static JE_VAL* _OP_SUBSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = JE_VecGet(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = JE_VecGet(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);
    left = _symtblGetByNode(table, ref->value.n, 1);

    if(left) {
        if     (left->type == JE_INT_V   && right->type == JE_INT_V  ) left->value.i -= right->value.i;
        else if(left->type == JE_INT_V   && right->type == JE_FLOAT_V) { right->value.f = left->value.i-right->value.f; JE_ValSwap(left, right); }
        else if(left->type == JE_FLOAT_V && right->type == JE_INT_V  ) left->value.f -= right->value.i;
        else if(left->type == JE_FLOAT_V && right->type == JE_FLOAT_V) left->value.f -= right->value.f;
        else left = NULL;
    }

    return left ? JE_ValDup(left) : JE_ValNewFromNull();
}

static JE_VAL* _OP_MULSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = JE_VecGet(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = JE_VecGet(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);
    left = _symtblGetByNode(table, ref->value.n, 1);

    if(left) {
        if     (left->type == JE_INT_V   && right->type == JE_INT_V  ) left->value.i *= right->value.i;
        else if(left->type == JE_INT_V   && right->type == JE_FLOAT_V) { right->value.f = left->value.i*right->value.f; JE_ValSwap(left, right); }
        else if(left->type == JE_FLOAT_V && right->type == JE_INT_V  ) left->value.f *= right->value.i;
        else if(left->type == JE_FLOAT_V && right->type == JE_FLOAT_V) left->value.f *= right->value.f;
        else left = NULL;
    }

    return left ? JE_ValDup(left) : JE_ValNewFromNull();
}

static JE_VAL* _OP_DIVSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = JE_VecGet(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = JE_VecGet(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);
    left = _symtblGetByNode(table, ref->value.n, 1);

    if(left) {
        if     (left->type == JE_INT_V   && right->type == JE_INT_V  ) left->value.i /= right->value.i;
        else if(left->type == JE_INT_V   && right->type == JE_FLOAT_V) { right->value.f = left->value.i/right->value.f; JE_ValSwap(left, right); }
        else if(left->type == JE_FLOAT_V && right->type == JE_INT_V  ) left->value.f /= right->value.i;
        else if(left->type == JE_FLOAT_V && right->type == JE_FLOAT_V) left->value.f /= right->value.f;
        else left = NULL;
    }

    return left ? JE_ValDup(left) : JE_ValNewFromNull();
}

static JE_VAL* _OP_MODSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = JE_VecGet(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = JE_VecGet(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);
    left = _symtblGetByNode(table, ref->value.n, 1);

    if(left) {
        if(left->type == JE_INT_V && right->type == JE_INT_V) left->value.i %= right->value.i;
        else if(left->type == JE_INT_V || left->type == JE_FLOAT_V) {
            double a = left->type==JE_INT_V ? left->value.i : left->value.f;
            double b = right->type==JE_INT_V ? right->value.i : right->value.f;
            JE_VAL* tmp = JE_ValNewFromFloat(a - floor(a / b)*b);

            JE_ValSwap(tmp, left);
            JE_ValDelete(tmp);
        }
        else left = NULL;
    }

    return left ? JE_ValDup(left) : JE_ValNewFromNull();
}

static JE_VAL* _OP_POWSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = JE_VecGet(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = JE_VecGet(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);
    left = _symtblGetByNode(table, ref->value.n, 1);

    if(left) {
        if(left->type == JE_INT_V || left->type == JE_FLOAT_V) {
            double a = left->type==JE_INT_V ? left->value.i : left->value.f;
            double b = right->type==JE_INT_V ? right->value.i : right->value.f;
            JE_VAL* tmp = JE_ValNewFromFloat(pow(a, b));

            JE_ValSwap(tmp, left);
            JE_ValDelete(tmp);
        }
        else left = NULL;
    }

    return left ? JE_ValDup(left) : JE_ValNewFromNull();
}

static JE_VAL* _OP_ORSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = JE_VecGet(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = JE_VecGet(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V);
    left = _symtblGetByNode(table, ref->value.n, 1);

    if(left) {
        if(left->type == JE_INT_V) left->value.i |= right->value.i;
        else left = NULL;
    }

    return left ? JE_ValDup(left) : JE_ValNewFromNull();
}

static JE_VAL* _OP_ANDSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = JE_VecGet(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = JE_VecGet(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V);
    left = _symtblGetByNode(table, ref->value.n, 1);

    if(left) {
        if(left->type == JE_INT_V) left->value.i &= right->value.i;
        else left = NULL;
    }

    return left ? JE_ValDup(left) : JE_ValNewFromNull();
}

static JE_VAL* _OP_XORSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = JE_VecGet(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = JE_VecGet(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V);
    left = _symtblGetByNode(table, ref->value.n, 1);

    if(left) {
        if(left->type == JE_INT_V) left->value.i ^= right->value.i;
        else left = NULL;
    }

    return left ? JE_ValDup(left) : JE_ValNewFromNull();
}

static JE_VAL* _OP_SHLSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = JE_VecGet(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = JE_VecGet(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V);
    left = _symtblGetByNode(table, ref->value.n, 1);

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

    return left ? JE_ValDup(left) : JE_ValNewFromNull();
}

static JE_VAL* _OP_ASRSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = JE_VecGet(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = JE_VecGet(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V);
    left = _symtblGetByNode(table, ref->value.n, 1);

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

    return left ? JE_ValDup(left) : JE_ValNewFromNull();
}

static JE_VAL* _OP_SHRSET(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* ref = JE_VecGet(args, 0);
    JE_VAL* left = NULL;
    JE_VAL* right = JE_VecGet(args, 1);

    assert(ref->type == JE_NODE_V);
    assert(right->type == JE_INT_V);
    left = _symtblGetByNode(table, ref->value.n, 1);

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

    return left ? JE_ValDup(left) : JE_ValNewFromNull();
}

static JE_VAL* _OP_DEREF(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* symnode = JE_VecGet(args, 0);
    JE_VAL* result = NULL;

    assert(symnode->type == JE_NODE_V);

    result = _symtblGetByNode(table, symnode->value.n, 0);

    return result ? JE_ValDup(result) : JE_ValNewFromNull();
}

static JE_VAL* _OP_INDEX(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);
    JE_VAL* result = NULL;

    if(left->type == JE_ARRAY_V && right->type == JE_INT_V) {
        result = JE_VecGet(left->value.v, right->value.i);
        if(!result) JeRuntimeError(loc, "Invalid index, max %zd, got %lld", left->value.v->length-1, right->value.i);
    }
    else if(left->type == JE_OBJECT_V && right->type == JE_STRING_V) {
        result = JE_MapGet(left->value.m, right->value.s);
        if(!result) JeRuntimeError(loc, "Invalid key, %s", JE_ValToQstr(right));
    }
    else if(left->type == JE_ARRAY_V ) JeRuntimeError(loc, "ARRAY index must be INTEGER but got %s", JE_ValGetType(right));
    else if(left->type == JE_OBJECT_V) JeRuntimeError(loc, "OBJECT key must be STRING but got %s", JE_ValGetType(right));
    else                               JeRuntimeError(loc, "ARRAY or OBJECT expected before '[' but got %s", JE_ValGetType(left));

    return result ? JE_ValDup(result) : JE_ValNewFromNull();
}

static JE_VAL* _OP_MEMBER(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_NODE* right = JE_VecGet(args, 1)->value.n;
    JE_VAL* result = NULL;

    if     (left->type == JE_OBJECT_V && right->type == JE_IDENT_N) result = JE_MapGet(left->value.m, right->value.s);
    else if(left->type == JE_OBJECT_V                             ) JeRuntimeError(loc, "IDENTIFIER expected after '.' but got %s", JE_NodeTypeCstr(right));
    else                                                            JeRuntimeError(loc, "OBJECT expected before '.' but got %s", JE_ValGetType(left));

    return result ? JE_ValDup(result) : JE_ValNewFromNull();
}

static JE_VAL* _OP_TERM(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    return JE_ValDup(JE_VecGet(args, 1));
}

static JE_VAL* _OP_CALL(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);
    JE_VEC* args2 = JE_VecNew();
    JE_VAL* result = NULL;

    assert(left->type == JE_FUNCTION_V);
    assert(right == NULL || right->type == JE_NODE_V);

    if(right) _unfold(args2, right->value.n, table);
    result = JE_FuncExec(left->value.fn, args2, table, loc);

    JE_VecDelete(args2);

    return result;
}

static JE_VAL* _OP_NOT(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);

    return JE_ValNewFromBool(!JE_ValIsTrue(left));
}

static JE_VAL* _OP_POS(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* result = NULL;

    assert(left->type == JE_INT_V || left->type == JE_FLOAT_V);

    switch(left->type) {
        case JE_INT_V   : result = JE_ValNewFromInt(left->value.i); break;
        case JE_FLOAT_V : result = JE_ValNewFromFloat(left->value.f); break;
        default         : result = JE_ValNewFromNull(); break;
    }

    return result ? result : JE_ValNewFromNull();
}

static JE_VAL* _OP_NEG(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* result = NULL;

    assert(left->type == JE_INT_V || left->type == JE_FLOAT_V);

    switch(left->type) {
        case JE_INT_V   : result = JE_ValNewFromInt(-left->value.i); break;
        case JE_FLOAT_V : result = JE_ValNewFromFloat(-left->value.f); break;
        default         : result = JE_ValNewFromNull(); break;
    }

    return result ? result : JE_ValNewFromNull();
}

static JE_VAL* _OP_INV(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);

    assert(left->type == JE_INT_V);

    return JE_ValNewFromInt(~left->value.i);
}

static JE_VAL* _OP_MUL(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);
    JE_VAL* result = NULL;

    assert(left->type == JE_INT_V || left->type == JE_FLOAT_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);

    if     (left->type == JE_INT_V   && right->type == JE_INT_V  ) result = JE_ValNewFromInt(left->value.i * right->value.i);
    else if(left->type == JE_INT_V   && right->type == JE_FLOAT_V) result = JE_ValNewFromFloat(left->value.i * right->value.f);
    else if(left->type == JE_FLOAT_V && right->type == JE_INT_V  ) result = JE_ValNewFromFloat(left->value.f * right->value.i);
    else if(left->type == JE_FLOAT_V && right->type == JE_FLOAT_V) result = JE_ValNewFromFloat(left->value.f * right->value.f);

    return result ? result : JE_ValNewFromNull();
}

static JE_VAL* _OP_DIV(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);
    JE_VAL* result = NULL;

    assert(left->type == JE_INT_V || left->type == JE_FLOAT_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);

    if     (left->type == JE_INT_V   && right->type == JE_INT_V  ) result = JE_ValNewFromInt(left->value.i / right->value.i);
    else if(left->type == JE_INT_V   && right->type == JE_FLOAT_V) result = JE_ValNewFromFloat(left->value.i / right->value.f);
    else if(left->type == JE_FLOAT_V && right->type == JE_INT_V  ) result = JE_ValNewFromFloat(left->value.f / right->value.i);
    else if(left->type == JE_FLOAT_V && right->type == JE_FLOAT_V) result = JE_ValNewFromFloat(left->value.f / right->value.f);

    return result ? result : JE_ValNewFromNull();
}

static JE_VAL* _OP_MOD(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);
    JE_VAL* result = NULL;

    assert(left->type == JE_INT_V || left->type == JE_FLOAT_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);

    if(left->type == JE_INT_V && right->type == JE_INT_V) result = JE_ValNewFromInt(left->value.i % right->value.i);
    else {
        double a = left->type == JE_INT_V ? left->value.i : left->value.f;
        double b = right->type == JE_INT_V ? right->value.i : right->value.f;

        result = JE_ValNewFromFloat(a - floor(a / b)*b);
    }

    return result ? result : JE_ValNewFromNull();
}

static JE_VAL* _OP_ADD(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);
    JE_VAL* result = NULL;

    if     (left->type == JE_INT_V    && right->type == JE_INT_V   ) result = JE_ValNewFromInt(left->value.i + right->value.i);
    else if(left->type == JE_INT_V    && right->type == JE_FLOAT_V ) result = JE_ValNewFromFloat(left->value.i + right->value.f);
    else if(left->type == JE_FLOAT_V  && right->type == JE_INT_V   ) result = JE_ValNewFromFloat(left->value.f + right->value.i);
    else if(left->type == JE_FLOAT_V  && right->type == JE_FLOAT_V ) result = JE_ValNewFromFloat(left->value.f + right->value.f);
    else if(left->type == JE_STRING_V || right->type == JE_STRING_V) {
        char* buf = NULL;

        asprintf(&buf, "%s%s", JE_ValToCstr(left), JE_ValToCstr(right));
        result = JE_ValNewFromCstr(buf);
        JE_Free(buf);
    }

    return result ? result : JE_ValNewFromNull();
}

static JE_VAL* _OP_SUB(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);
    JE_VAL* result = NULL;

    assert(left->type == JE_INT_V || left->type == JE_FLOAT_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);

    if     (left->type == JE_INT_V   && right->type == JE_INT_V  ) result = JE_ValNewFromInt(left->value.i - right->value.i);
    else if(left->type == JE_INT_V   && right->type == JE_FLOAT_V) result = JE_ValNewFromFloat(left->value.i - right->value.f);
    else if(left->type == JE_FLOAT_V && right->type == JE_INT_V  ) result = JE_ValNewFromFloat(left->value.f - right->value.i);
    else if(left->type == JE_FLOAT_V && right->type == JE_FLOAT_V) result = JE_ValNewFromFloat(left->value.f - right->value.f);

    return result ? result : JE_ValNewFromNull();
}

static JE_VAL* _OP_POW(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);
    JE_VAL* result = NULL;

    assert(left->type == JE_INT_V || left->type == JE_FLOAT_V);
    assert(right->type == JE_INT_V || right->type == JE_FLOAT_V);

    if     (left->type == JE_INT_V   && right->type == JE_INT_V  ) result = JE_ValNewFromFloat(pow(left->value.i, right->value.i));
    else if(left->type == JE_INT_V   && right->type == JE_FLOAT_V) result = JE_ValNewFromFloat(pow(left->value.i, right->value.f));
    else if(left->type == JE_FLOAT_V && right->type == JE_INT_V  ) result = JE_ValNewFromFloat(pow(left->value.f, right->value.i));
    else if(left->type == JE_FLOAT_V && right->type == JE_FLOAT_V) result = JE_ValNewFromFloat(pow(left->value.f, right->value.f));

    return result ? result : JE_ValNewFromNull();
}

static JE_VAL* _OP_OR(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);

    assert(left->type == JE_INT_V);
    assert(right->type == JE_INT_V);

    return JE_ValNewFromInt(left->value.i | right->value.i);
}

static JE_VAL* _OP_AND(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);

    assert(left->type == JE_INT_V);
    assert(right->type == JE_INT_V);

    return JE_ValNewFromInt(left->value.i & right->value.i);
}

static JE_VAL* _OP_XOR(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);

    assert(left->type == JE_INT_V);
    assert(right->type == JE_INT_V);

    return JE_ValNewFromInt(left->value.i ^ right->value.i);
}

static JE_VAL* _OP_LT(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);

    return JE_ValNewFromBool(JE_ValCmp(left, right) < 0);
}

static JE_VAL* _OP_GT(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);

    return JE_ValNewFromBool(JE_ValCmp(left, right) > 0);
}

static JE_VAL* _OP_EQ(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);

    return JE_ValNewFromBool(JE_ValCmp(left, right) == 0);
}

static JE_VAL* _OP_NE(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);

    return JE_ValNewFromBool(JE_ValCmp(left, right) != 0);
}

static JE_VAL* _OP_LE(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);

    return JE_ValNewFromBool(JE_ValCmp(left, right) <= 0);
}

static JE_VAL* _OP_GE(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);

    return JE_ValNewFromBool(JE_ValCmp(left, right) >= 0);
}

static JE_VAL* _OP_LOR(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);
    JE_VAL* result = NULL;

    assert(left->type == JE_NODE_V);
    assert(right->type == JE_NODE_V);

    result = JE_EvalByNode(left->value.n, table);

    if(!JE_ValIsTrue(result)) {
        JE_ValDelete(result);
        result = JE_EvalByNode(right->value.n, table);
    }

    return result;
}

static JE_VAL* _OP_LAND(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);
    JE_VAL* result = NULL;

    assert(left->type == JE_NODE_V);
    assert(right->type == JE_NODE_V);

    result = JE_EvalByNode(left->value.n, table);

    if(JE_ValIsTrue(result)) {
        JE_ValDelete(result);
        result = JE_EvalByNode(right->value.n, table);
    }

    return result;
}

static JE_VAL* _OP_SHL(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);
    JE_VAL* result = NULL;
    int64_t a = left->value.i;
    int64_t b = right->value.i;

    assert(left->type == JE_INT_V);
    assert(right->type == JE_INT_V);

    if     (b <-63 && a < 0) result = JE_ValNewFromInt(-1);
    else if(b <-63         ) result = JE_ValNewFromInt(0);
    else if(b <  0 && a < 0) result = JE_ValNewFromInt(~(~a >> -b));
    else if(b <  0         ) result = JE_ValNewFromInt(a >> -b);
    else if(b == 0         ) result = JE_ValNewFromInt(a);
    else if(b < 63         ) result = JE_ValNewFromInt(a << b);
    else                     result = JE_ValNewFromInt(0);

    return result ? result : JE_ValNewFromNull();
}

static JE_VAL* _OP_ASR(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);
    JE_VAL* result = NULL;
    int64_t a = left->value.i;
    int64_t b = right->value.i;

    assert(left->type == JE_INT_V);
    assert(right->type == JE_INT_V);

    if     (b <-63         ) result = JE_ValNewFromInt(0);
    else if(b <  0         ) result = JE_ValNewFromInt(a << -b);
    else if(b == 0         ) result = JE_ValNewFromInt(a);
    else if(b < 63 && a < 0) result = JE_ValNewFromInt(~(~a >> b));
    else if(b < 63         ) result = JE_ValNewFromInt(a >> b);
    else if(          a < 0) result = JE_ValNewFromInt(-1);
    else                     result = JE_ValNewFromInt(0);

    return result ? result : JE_ValNewFromNull();
}

static JE_VAL* _OP_SHR(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);
    JE_VAL* result = NULL;
    uint64_t a = left->value.i;
    int64_t b = right->value.i;

    assert(left->type == JE_INT_V);
    assert(right->type == JE_INT_V);

    if     (b <-63) result = JE_ValNewFromInt(0);
    else if(b <  0) result = JE_ValNewFromInt(a << -b);
    else if(b == 0) result = JE_ValNewFromInt(a);
    else if(b < 63) result = JE_ValNewFromInt(a >> b);
    else            result = JE_ValNewFromInt(0);

    return result ? result : JE_ValNewFromNull();
}

static JE_VAL* _OP_COND(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc) {
    JE_VAL* left = JE_VecGet(args, 0);
    JE_VAL* right = JE_VecGet(args, 1);
    JE_VAL* righter = JE_VecGet(args, 2);
    JE_VAL* result = NULL;

    assert(left->type == JE_NODE_V);
    assert(right->type == JE_NODE_V);
    assert(righter->type == JE_NODE_V);

    result = JE_EvalByNode(left->value.n, table);

    if(JE_ValIsTrue(result)) {
        JE_ValDelete(result);
        result = JE_EvalByNode(right->value.n, table);
    }
    else {
        JE_ValDelete(result);
        result = JE_EvalByNode(righter->value.n, table);
    }

    return result;
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_MAP* JE_BuiltinFns() {
    if(!_BINFNS) {
        _BINFNS = JE_MapNew();

        JE_MapSet(_BINFNS, "VERSION", JE_ValNewFromCstr(JE_VERSION));
        JE_MapSet(_BINFNS, "VERSION_MAJOR", JE_ValNewFromInt(JE_VERSION_MAJOR));
        JE_MapSet(_BINFNS, "VERSION_MINOR", JE_ValNewFromInt(JE_VERSION_MINOR));
        JE_MapSet(_BINFNS, "VERSION_PATCH", JE_ValNewFromInt(JE_VERSION_PATCH));

        _addtomap(_BINFNS, "CEIL"    , "#"   , _CEIL    );
        _addtomap(_BINFNS, "EVAL"    , "S"   , _EVAL    );
        _addtomap(_BINFNS, "FLOOR"   , "#"   , _FLOOR   );
        _addtomap(_BINFNS, "FOR"     , "....", _FOR     );
        _addtomap(_BINFNS, "FOREACH" , ".@." , _FOREACH );
        _addtomap(_BINFNS, "FUNCTION", "S."  , _FUNCTION);
        _addtomap(_BINFNS, "IF"      , ".**" , _IF      );
        _addtomap(_BINFNS, "LEN"     , "?"   , _LEN     );
        _addtomap(_BINFNS, "PRINT"   , "*"   , _PRINT   );
        _addtomap(_BINFNS, "ROUND"   , "#"   , _ROUND   );
        _addtomap(_BINFNS, "SQRT"    , "?"   , _SQRT    );
        _addtomap(_BINFNS, "WHILE"   , ".."  , _WHILE   );
    }

    return _BINFNS;
}

JE_MAP* JE_BuiltinOps() {
    if(!_BINOPS) {
        _BINOPS = JE_MapNew();

        _addtomap(_BINOPS, "++x" , "."  , _OP_INC     );
        _addtomap(_BINOPS, "--x" , "."  , _OP_DEC     );
        _addtomap(_BINOPS, "x++" , "."  , _OP_INCPOST );
        _addtomap(_BINOPS, "x--" , "."  , _OP_DECPOST );

        _addtomap(_BINOPS, "="   , ".?" , _OP_SET     );
        _addtomap(_BINOPS, "+="  , ".?" , _OP_ADDSET  );
        _addtomap(_BINOPS, "-="  , ".#" , _OP_SUBSET  );
        _addtomap(_BINOPS, "*="  , ".#" , _OP_MULSET  );
        _addtomap(_BINOPS, "/="  , ".#" , _OP_DIVSET  );
        _addtomap(_BINOPS, "%="  , ".#" , _OP_MODSET  );
        _addtomap(_BINOPS, "**=" , ".#" , _OP_POWSET  );

        _addtomap(_BINOPS, "|="  , ".I" , _OP_ORSET   );
        _addtomap(_BINOPS, "&="  , ".I" , _OP_ANDSET  );
        _addtomap(_BINOPS, "^="  , ".I" , _OP_XORSET  );
        _addtomap(_BINOPS, "<<=" , ".I" , _OP_SHLSET  );
        _addtomap(_BINOPS, ">>=" , ".I" , _OP_ASRSET  );
        _addtomap(_BINOPS, ">>>=", ".I" , _OP_SHRSET  );

        _addtomap(_BINOPS, "*x"  , "."  , _OP_DEREF   );
        _addtomap(_BINOPS, "[]"  , "??" , _OP_INDEX   );
        _addtomap(_BINOPS, "."   , "?." , _OP_MEMBER  );
        _addtomap(_BINOPS, ";"   , "??" , _OP_TERM    );
        _addtomap(_BINOPS, "()"  , "F**", _OP_CALL    );

        _addtomap(_BINOPS, "!"   , "?"  , _OP_NOT     );
        _addtomap(_BINOPS, "~"   , "I"  , _OP_INV     );
        _addtomap(_BINOPS, "+x"  , "#"  , _OP_POS     );
        _addtomap(_BINOPS, "-x"  , "#"  , _OP_NEG     );

        _addtomap(_BINOPS, "*"   , "##" , _OP_MUL     );
        _addtomap(_BINOPS, "/"   , "##" , _OP_DIV     );
        _addtomap(_BINOPS, "%"   , "##" , _OP_MOD     );
        _addtomap(_BINOPS, "+"   , "??" , _OP_ADD     );
        _addtomap(_BINOPS, "-"   , "##" , _OP_SUB     );
        _addtomap(_BINOPS, "|"   , "II" , _OP_OR      );
        _addtomap(_BINOPS, "&"   , "II" , _OP_AND     );
        _addtomap(_BINOPS, "^"   , "II" , _OP_XOR     );
        _addtomap(_BINOPS, "<"   , "??" , _OP_LT      );
        _addtomap(_BINOPS, ">"   , "??" , _OP_GT      );
        _addtomap(_BINOPS, "**"  , "##" , _OP_POW     );
        _addtomap(_BINOPS, "=="  , "??" , _OP_EQ      );
        _addtomap(_BINOPS, "!="  , "??" , _OP_NE      );
        _addtomap(_BINOPS, "<="  , "??" , _OP_LE      );
        _addtomap(_BINOPS, ">="  , "??" , _OP_GE      );
        _addtomap(_BINOPS, "||"  , ".." , _OP_LOR     );
        _addtomap(_BINOPS, "&&"  , ".." , _OP_LAND    );
        _addtomap(_BINOPS, "<<"  , "II" , _OP_SHL     );
        _addtomap(_BINOPS, ">>"  , "II" , _OP_ASR     );
        _addtomap(_BINOPS, ">>>" , "II" , _OP_SHR     );

        _addtomap(_BINOPS, "?:"  , "...", _OP_COND    );
    }

    return _BINOPS;
}

JE_VAL* JE_BuiltinOpExec(const char* key, JE_SYMTBL* table, JE_NODE* left, JE_NODE* right, JE_NODE* righter) {
    JE_VAL* fnval = JE_MapGet(JE_BuiltinOps(), key);
    JE_VAL* result = NULL;

    if(fnval && fnval->type == JE_FUNCTION_V) {
        JE_FUNC* fn = fnval->value.fn;
        JE_VEC* args = JE_VecNew();

        if(left) JE_VecPush(args, JE_ValNewFromNode(left));
        if(right) JE_VecPush(args, JE_ValNewFromNode(right));
        if(righter) JE_VecPush(args, JE_ValNewFromNode(righter));

        result = JE_FuncExec(fn, args, table, &left->loc);
        JE_VecDelete(args);
    }

    return result ? result : JE_ValNewFromNull();
}
