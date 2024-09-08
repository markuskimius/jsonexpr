#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "builtin.h"
#include "error.h"
#include "eval.h"
#include "func.h"
#include "map.h"
#include "val.h"
#include "vec.h"
#include "node.h"
#include "symtbl.h"
#include "util.h"


/* ***************************************************************************
* TYPES
*/

typedef struct USERFN {
    NODE* body;
} USERFN;


/* ***************************************************************************
* GLOBALS
*/

static MAP* BINFNS = NULL;
static MAP* BINOPS = NULL;
static size_t ncustfunc = 0;


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static void addfn(MAP* map, const char* key, const char* sig, BINARY_FN fn) {
    mapset(map, key, funcval(newfunc(fn, key, sig)));
}


static VAL* nullify(VAL* value) {
    VAL* tmp = nullval();

    swapval(value, tmp);
    freeval(tmp);

    return value;
}


static VAL* tablemod(SYMTBL* table, NODE* node, int create) {
    VAL* val = NULL;

    switch(node->type) {
        case SYMBOL_N:
            val = tablemod(table, node->left, create);
            break;

        case IDENT_N:
            val = tableget(table, node->value.s);

            if(!val && create) {
                val = nullval();
                tableset(table, node->value.s, val);
            }

            break;

        case '[': {
            VAL* left = tablemod(table, node->left, create);
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
            VAL* left = tablemod(table, node->left, create);
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


static void unfold(VEC* vec, NODE* node, SYMTBL* table) {
    switch(node->type) {
        case ',':
            if(node->left) unfold(vec, node->left, table);
            if(node->right) unfold(vec, node->right, table);
            break;

        default:
            vecpush(vec, nodeval(node));
            break;
    }
}


/* ***************************************************************************
* BUILT-IN FUNCTIONS
*/

static VAL* IF(VEC* args, SYMTBL* table) {
    VAL* result = NULL;

    for(size_t i=0; i<(args->length & ~1UL); i+=2) {
        VAL* cond = eval(args->item[i]->value.n, table);

        if(valtrue(cond)) {
            result = eval(args->item[i+1]->value.n, table);
            break;
        }
    }

    if(!result && (args->length % 2)) {
        result = eval(args->item[args->length-1]->value.n, table);
    }

    return result ? result : nullval();
}


static VAL* FOR(VEC* args, SYMTBL* table) {
    VAL* result = NULL;
    VAL* last = eval(args->item[0]->value.n, table);

    while(1) {
        freeval(last);
        last = eval(args->item[1]->value.n, table);

        if(!valtrue(last)) break;
        result = eval(args->item[3]->value.n, table);

        freeval(last);
        last = eval(args->item[2]->value.n, table);
    }

    freeval(last);

    return result ? result : nullval();
}


static VAL* LEN(VEC* args, SYMTBL* table) {
    VAL* result = NULL;
    VAL* val = args->item[0];

    switch(val->type) {
        case STRING_V   : result = intval(strlen(valstr(val))); break;
        case ARRAY_V    : result = intval(val->value.v->length); break;
        case OBJECT_V   : result = intval(val->value.m->length); break;
        default         : throwLater("Type has no length: '%c' (%d)", val->type, val->type); break;
    }

    return result;
}


static VAL* SQRT(VEC* args, SYMTBL* table) {
    VAL* result = NULL;
    VAL* val = args->item[0];

    switch(val->type) {
        case INT_V      : result = dblval(sqrt(val->value.i)); break;
        case FLOAT_V    : result = dblval(sqrt(val->value.f)); break;
        default         : throwLater("Invalid argument to SQRT(): '%c' (%d)", val->type, val->type); break;
    }

    return result ? result : nullval();
}


static VAL* PRINT(VEC* args, SYMTBL* table) {
    VAL* last = NULL;

    for(size_t i=0; i<args->length; i++) {
        if(last) freeval(last);
        last = dupval(args->item[i]);

        if(i > 0) printf(" ");
        printf("%s", valstr(last));
    }

    printf("\n");

    return last ? last : nullval();
}


static VAL* FUNCTION(VEC* args, SYMTBL* table) {
    VAL* spec = args->item[0];
    char* sig = valstr(spec);
    int isok = 1;

    if(spec->type == STRING_V) {
        int nstars = 0;

        /* Validate the signature argument */
        for(size_t i=0; i<strlen(sig); i++) {
            /* Validate signature */
            if(!strchr("BIDSAOF?.*", sig[i])) {
                throwLater("Invalid function argument signature: %c", sig[i]);
                isok = 0;
                break;
            }

            /* Signature can only end with a star */
            if(nstars && sig[i] != '*') {
                throwLater("'*' must terminate function argument signature");
                isok = 0;
                break;
            }

            /* Maximum # of stars is 2 */
            if(sig[i] == '*' && nstars == 2) {
                throwLater("Too many '*', maximum is 2");
                isok = 0;
                break;
            }

            if(sig[i] == '*') nstars++;
        }
    }
    else {
        throwLater("Invalid argument type, expected string but got %c", spec->type);
        isok = 0;
    }

    if(isok) {
        char name[64];

        snprintf(name, sizeof(name), "FUNCTION#%zd()", ++ncustfunc);

        return funcval(newcustfunc(args->item[1]->value.n, name, sig, table));
    }

    return NULL;
}


/* ***************************************************************************
* BUILT-IN OPERATIONS
*/

static VAL* OP_INC(VEC* args, SYMTBL* table) {
    VAL* ref = vecget(args, 0);
    VAL* left = NULL;
    VAL* result = NULL;

    assert(ref->type == NODE_V);
    left = tablemod(table, ref->value.n, 0);

    if(left) {
        switch(left->type) {
            case INT_V   : result = intval(left->value.i += 1); break;
            case FLOAT_V : result = dblval(left->value.f += 1.0); break;
            default      : result = dupval(nullify(left)); break;
        }
    }

    return result ? result : nullval();
}


static VAL* OP_DEC(VEC* args, SYMTBL* table) {
    VAL* ref = vecget(args, 0);
    VAL* left = NULL;
    VAL* result = NULL;

    assert(ref->type == NODE_V);
    left = tablemod(table, ref->value.n, 0);

    if(left) {
        switch(left->type) {
            case INT_V   : result = intval(left->value.i -= 1); break;
            case FLOAT_V : result = dblval(left->value.f -= 1.0); break;
            default      : result = dupval(nullify(left)); break;
        }
    }

    return result ? result : nullval();
}


static VAL* OP_INCPOST(VEC* args, SYMTBL* table) {
    VAL* ref = vecget(args, 0);
    VAL* left = NULL;
    VAL* result = NULL;

    assert(ref->type == NODE_V);
    left = tablemod(table, ref->value.n, 0);

    if(left) {
        switch(left->type) {
            case INT_V   : result = intval(left->value.i); left->value.i += 1; break;
            case FLOAT_V : result = dblval(left->value.f); left->value.f += 1.0; break;
            default      : result = dupval(nullify(left)); break;
        }
    }

    return result ? result : nullval();
}


static VAL* OP_DECPOST(VEC* args, SYMTBL* table) {
    VAL* ref = vecget(args, 0);
    VAL* left = NULL;
    VAL* result = NULL;

    assert(ref->type == NODE_V);
    left = tablemod(table, ref->value.n, 0);

    if(left) {
        switch(left->type) {
            case INT_V   : result = intval(left->value.i); left->value.i -= 1; break;
            case FLOAT_V : result = dblval(left->value.f); left->value.f -= 1.0; break;
            default      : result = dupval(nullify(left)); break;
        }
    }

    return result ? result : nullval();
}


static VAL* OP_SET(VEC* args, SYMTBL* table) {
    VAL* ref = vecget(args, 0);
    VAL* left = NULL;
    VAL* right = vecget(args, 1);

    assert(ref->type == NODE_V);
    left = tablemod(table, ref->value.n, 1);
    swapval(left, right);

    return dupval(left);
}


static VAL* OP_ADDSET(VEC* args, SYMTBL* table) {
    VAL* ref = vecget(args, 0);
    VAL* left = NULL;
    VAL* right = vecget(args, 1);

    assert(ref->type == NODE_V);
    left = tablemod(table, ref->value.n, 1);

    if     (left->type == INT_V    && right->type == INT_V   ) left->value.i += right->value.i;
    else if(left->type == INT_V    && right->type == FLOAT_V ) { right->value.f += left->value.i; swapval(left, right); }
    else if(left->type == FLOAT_V  && right->type == INT_V   ) left->value.f += right->value.i;
    else if(left->type == FLOAT_V  && right->type == FLOAT_V ) left->value.f += right->value.f;
    else if(left->type == STRING_V                           ) left->value.s = astrcat(left->value.s, valstr(right));
    else if(                          right->type == STRING_V) {
        VAL* tmp = strval("");

        tmp->value.s = casprintf(tmp->value.s, "%s%s", valstr(left), right->value.s);
        swapval(tmp, left);
        freeval(tmp);
    }
    else nullify(left);

    return dupval(left);
}


static VAL* OP_SUBSET(VEC* args, SYMTBL* table) {
    VAL* ref = vecget(args, 0);
    VAL* left = NULL;
    VAL* right = vecget(args, 1);

    assert(ref->type == NODE_V);
    assert(right->type == INT_V || right->type == FLOAT_V);
    left = tablemod(table, ref->value.n, 1);

    if     (left->type == INT_V   && right->type == INT_V  ) left->value.i -= right->value.i;
    else if(left->type == INT_V   && right->type == FLOAT_V) { right->value.f = left->value.i-right->value.f; swapval(left, right); }
    else if(left->type == FLOAT_V && right->type == INT_V  ) left->value.f -= right->value.i;
    else if(left->type == FLOAT_V && right->type == FLOAT_V) left->value.f -= right->value.f;
    else nullify(left);

    return dupval(left);
}


static VAL* OP_MULSET(VEC* args, SYMTBL* table) {
    VAL* ref = vecget(args, 0);
    VAL* left = NULL;
    VAL* right = vecget(args, 1);

    assert(ref->type == NODE_V);
    assert(right->type == INT_V || right->type == FLOAT_V);
    left = tablemod(table, ref->value.n, 1);

    if     (left->type == INT_V   && right->type == INT_V  ) left->value.i *= right->value.i;
    else if(left->type == INT_V   && right->type == FLOAT_V) { right->value.f = left->value.i*right->value.f; swapval(left, right); }
    else if(left->type == FLOAT_V && right->type == INT_V  ) left->value.f *= right->value.i;
    else if(left->type == FLOAT_V && right->type == FLOAT_V) left->value.f *= right->value.f;
    else nullify(left);

    return dupval(left);
}


static VAL* OP_DIVSET(VEC* args, SYMTBL* table) {
    VAL* ref = vecget(args, 0);
    VAL* left = NULL;
    VAL* right = vecget(args, 1);

    assert(ref->type == NODE_V);
    assert(right->type == INT_V || right->type == FLOAT_V);
    left = tablemod(table, ref->value.n, 1);

    if     (left->type == INT_V   && right->type == INT_V  ) left->value.i /= right->value.i;
    else if(left->type == INT_V   && right->type == FLOAT_V) { right->value.f = left->value.i/right->value.f; swapval(left, right); }
    else if(left->type == FLOAT_V && right->type == INT_V  ) left->value.f /= right->value.i;
    else if(left->type == FLOAT_V && right->type == FLOAT_V) left->value.f /= right->value.f;
    else nullify(left);

    return dupval(left);
}


static VAL* OP_MODSET(VEC* args, SYMTBL* table) {
    VAL* ref = vecget(args, 0);
    VAL* left = NULL;
    VAL* right = vecget(args, 1);

    assert(ref->type == NODE_V);
    assert(right->type == INT_V || right->type == FLOAT_V);
    left = tablemod(table, ref->value.n, 1);

    if(left->type == INT_V && right->type == INT_V) left->value.i %= right->value.i;
    else if(left->type == INT_V || left->type == FLOAT_V) {
        double a = left->type==INT_V ? left->value.i : left->value.f;
        double b = right->type==INT_V ? right->value.i : right->value.f;
        VAL* tmp = dblval(a - floor(a / b)*b);

        swapval(tmp, left);
        freeval(tmp);
    }
    else nullify(left);

    return dupval(left);
}


static VAL* OP_POWSET(VEC* args, SYMTBL* table) {
    VAL* ref = vecget(args, 0);
    VAL* left = NULL;
    VAL* right = vecget(args, 1);

    assert(ref->type == NODE_V);
    assert(right->type == INT_V || right->type == FLOAT_V);
    left = tablemod(table, ref->value.n, 1);

    if(left->type == INT_V || left->type == FLOAT_V) {
        double a = left->type==INT_V ? left->value.i : left->value.f;
        double b = right->type==INT_V ? right->value.i : right->value.f;
        VAL* tmp = dblval(pow(a, b));

        swapval(tmp, left);
        freeval(tmp);
    }
    else nullify(left);

    return dupval(left);
}

static VAL* OP_ORSET(VEC* args, SYMTBL* table) {
    VAL* ref = vecget(args, 0);
    VAL* left = NULL;
    VAL* right = vecget(args, 1);

    assert(ref->type == NODE_V);
    assert(right->type == INT_V);
    left = tablemod(table, ref->value.n, 1);

    if(left->type == INT_V) left->value.i |= right->value.i;
    else nullify(left);

    return dupval(left);
}


static VAL* OP_ANDSET(VEC* args, SYMTBL* table) {
    VAL* ref = vecget(args, 0);
    VAL* left = NULL;
    VAL* right = vecget(args, 1);

    assert(ref->type == NODE_V);
    assert(right->type == INT_V);
    left = tablemod(table, ref->value.n, 1);

    if(left->type == INT_V) left->value.i &= right->value.i;
    else nullify(left);

    return dupval(left);
}


static VAL* OP_XORSET(VEC* args, SYMTBL* table) {
    VAL* ref = vecget(args, 0);
    VAL* left = NULL;
    VAL* right = vecget(args, 1);

    assert(ref->type == NODE_V);
    assert(right->type == INT_V);
    left = tablemod(table, ref->value.n, 1);

    if(left->type == INT_V) left->value.i ^= right->value.i;
    else nullify(left);

    return dupval(left);
}


static VAL* OP_SHLSET(VEC* args, SYMTBL* table) {
    VAL* ref = vecget(args, 0);
    VAL* left = NULL;
    VAL* right = vecget(args, 1);

    assert(ref->type == NODE_V);
    assert(right->type == INT_V);
    left = tablemod(table, ref->value.n, 1);

    if(left->type == INT_V) {
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
    else nullify(left);

    return dupval(left);
}


static VAL* OP_ASRSET(VEC* args, SYMTBL* table) {
    VAL* ref = vecget(args, 0);
    VAL* left = NULL;
    VAL* right = vecget(args, 1);

    assert(ref->type == NODE_V);
    assert(right->type == INT_V);
    left = tablemod(table, ref->value.n, 1);

    if(left->type == INT_V) {
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
    else nullify(left);

    return dupval(left);
}


static VAL* OP_SHRSET(VEC* args, SYMTBL* table) {
    VAL* ref = vecget(args, 0);
    VAL* left = NULL;
    VAL* right = vecget(args, 1);

    assert(ref->type == NODE_V);
    assert(right->type == INT_V);
    left = tablemod(table, ref->value.n, 1);

    if(left->type == INT_V) {
        uint64_t a = left->value.i;
        int64_t b = right->value.i;

        if     (b <-63) left->value.i = 0;
        else if(b <  0) left->value.i <<= -b;
        else if(b == 0) /* no op */ ;
        else if(b < 63) left->value.i = (a >> b);
        else            left->value.i = 0;
    }
    else nullify(left);

    return dupval(left);
}


static VAL* OP_DEREF(VEC* args, SYMTBL* table) {
    VAL* to = vecget(args, 0);
    VAL* result = NULL;

    assert(to->type == NODE_V);

    result = tablemod(table, to->value.n, 0);

    return result ? dupval(result) : nullval();
}


static VAL* OP_TERM(VEC* args, SYMTBL* table) {
    return dupval(vecget(args, 1));
}


static VAL* OP_CALL(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);
    VEC* args2 = newvec();
    VAL* result = NULL;

    assert(left->type == FUNCTION_V);
    assert(right == NULL || right->type == NODE_V);

    if(right) unfold(args2, right->value.n, table);
    result = funcexec(left->value.fn, args2, table);

    freevec(args2);

    return result ? result : nullval();
}


static VAL* OP_NOT(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);

    return boolval(!valtrue(left));
}


static VAL* OP_POS(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* result = NULL;

    assert(left->type == INT_V || left->type == FLOAT_V);

    switch(left->type) {
        case INT_V   : result = intval(left->value.i); break;
        case FLOAT_V : result = dblval(left->value.f); break;
        default      : result = nullval(); break;
    }

    return result ? result : nullval();
}


static VAL* OP_NEG(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* result = NULL;

    assert(left->type == INT_V || left->type == FLOAT_V);

    switch(left->type) {
        case INT_V   : result = intval(-left->value.i); break;
        case FLOAT_V : result = dblval(-left->value.f); break;
        default      : result = nullval(); break;
    }

    return result ? result : nullval();
}


static VAL* OP_INV(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);

    assert(left->type == INT_V);

    return intval(~left->value.i);
}


static VAL* OP_MUL(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);
    VAL* result = NULL;

    assert(left->type == INT_V || left->type == FLOAT_V);
    assert(right->type == INT_V || right->type == FLOAT_V);

    if     (left->type == INT_V   && right->type == INT_V  ) result = intval(left->value.i * right->value.i);
    else if(left->type == INT_V   && right->type == FLOAT_V) result = dblval(left->value.i * right->value.f);
    else if(left->type == FLOAT_V && right->type == INT_V  ) result = dblval(left->value.f * right->value.i);
    else if(left->type == FLOAT_V && right->type == FLOAT_V) result = dblval(left->value.f * right->value.f);

    return result ? result : nullval();
}


static VAL* OP_DIV(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);
    VAL* result = NULL;

    assert(left->type == INT_V || left->type == FLOAT_V);
    assert(right->type == INT_V || right->type == FLOAT_V);

    if     (left->type == INT_V   && right->type == INT_V  ) result = intval(left->value.i / right->value.i);
    else if(left->type == INT_V   && right->type == FLOAT_V) result = dblval(left->value.i / right->value.f);
    else if(left->type == FLOAT_V && right->type == INT_V  ) result = dblval(left->value.f / right->value.i);
    else if(left->type == FLOAT_V && right->type == FLOAT_V) result = dblval(left->value.f / right->value.f);

    return result ? result : nullval();
}


static VAL* OP_MOD(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);
    VAL* result = NULL;

    assert(left->type == INT_V || left->type == FLOAT_V);
    assert(right->type == INT_V || right->type == FLOAT_V);

    if(left->type == INT_V && right->type == INT_V) result = intval(left->value.i % right->value.i);
    else {
        double a = left->type == INT_V ? left->value.i : left->value.f;
        double b = right->type == INT_V ? right->value.i : right->value.f;

        result = dblval(a - floor(a / b)*b);
    }

    return result ? result : nullval();
}


static VAL* OP_ADD(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);
    VAL* result = NULL;

    if     (left->type == INT_V    && right->type == INT_V   ) result = intval(left->value.i / right->value.i);
    else if(left->type == INT_V    && right->type == FLOAT_V ) result = dblval(left->value.i / right->value.f);
    else if(left->type == FLOAT_V  && right->type == INT_V   ) result = dblval(left->value.f / right->value.i);
    else if(left->type == FLOAT_V  && right->type == FLOAT_V ) result = dblval(left->value.f / right->value.f);
    else if(left->type == STRING_V || right->type == STRING_V) {
        char* buf = NULL;

        asprintf(&buf, "%s%s", valstr(left), valstr(right));
        result = strval(buf);
        free(buf);
    }

    return result ? result : nullval();
}


static VAL* OP_SUB(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);
    VAL* result = NULL;

    assert(left->type == INT_V || left->type == FLOAT_V);
    assert(right->type == INT_V || right->type == FLOAT_V);

    if     (left->type == INT_V   && right->type == INT_V  ) result = intval(left->value.i - right->value.i);
    else if(left->type == INT_V   && right->type == FLOAT_V) result = dblval(left->value.i - right->value.f);
    else if(left->type == FLOAT_V && right->type == INT_V  ) result = dblval(left->value.f - right->value.i);
    else if(left->type == FLOAT_V && right->type == FLOAT_V) result = dblval(left->value.f - right->value.f);

    return result ? result : nullval();
}


static VAL* OP_POW(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);
    VAL* result = NULL;

    assert(left->type == INT_V || left->type == FLOAT_V);
    assert(right->type == INT_V || right->type == FLOAT_V);

    if     (left->type == INT_V   && right->type == INT_V  ) result = dblval(pow(left->value.i, right->value.i));
    else if(left->type == INT_V   && right->type == FLOAT_V) result = dblval(pow(left->value.i, right->value.f));
    else if(left->type == FLOAT_V && right->type == INT_V  ) result = dblval(pow(left->value.f, right->value.i));
    else if(left->type == FLOAT_V && right->type == FLOAT_V) result = dblval(pow(left->value.f, right->value.f));

    return result ? result : nullval();
}


static VAL* OP_OR(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);

    assert(left->type == INT_V);
    assert(right->type == INT_V);

    return intval(left->value.i | right->value.i);
}


static VAL* OP_AND(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);

    assert(left->type == INT_V);
    assert(right->type == INT_V);

    return intval(left->value.i & right->value.i);
}


static VAL* OP_XOR(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);

    assert(left->type == INT_V);
    assert(right->type == INT_V);

    return intval(left->value.i ^ right->value.i);
}


static VAL* OP_LT(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);

    return boolval(valcmp(left, right) < 0);
}


static VAL* OP_GT(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);

    return boolval(valcmp(left, right) > 0);
}


static VAL* OP_EQ(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);

    return boolval(valcmp(left, right) == 0);
}


static VAL* OP_NE(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);

    return boolval(valcmp(left, right) != 0);
}


static VAL* OP_LE(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);

    return boolval(valcmp(left, right) <= 0);
}


static VAL* OP_GE(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);

    return boolval(valcmp(left, right) >= 0);
}


static VAL* OP_LOR(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);
    VAL* result = NULL;

    assert(left->type == NODE_V);
    assert(right->type == NODE_V);

    result = eval(left->value.n, table);

    if(!valtrue(result)) {
        freeval(result);
        result = eval(right->value.n, table);
    }

    return result;
}


static VAL* OP_LAND(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);
    VAL* result = NULL;

    assert(left->type == NODE_V);
    assert(right->type == NODE_V);

    result = eval(left->value.n, table);

    if(valtrue(result)) {
        freeval(result);
        result = eval(right->value.n, table);
    }

    return result;
}


static VAL* OP_SHL(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);
    VAL* result = NULL;
    int64_t a = left->value.i;
    int64_t b = right->value.i;

    assert(left->type == INT_V);
    assert(right->type == INT_V);

    if     (b <-63 && a < 0) result = intval(-1);
    else if(b <-63         ) result = intval(0);
    else if(b <  0 && a < 0) result = intval(~(~a >> -b));
    else if(b <  0         ) result = intval(a >> -b);
    else if(b == 0         ) result = intval(a);
    else if(b < 63         ) result = intval(a << b);
    else                     result = intval(0);

    return result ? result : nullval();
}


static VAL* OP_ASR(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);
    VAL* result = NULL;
    int64_t a = left->value.i;
    int64_t b = right->value.i;

    assert(left->type == INT_V);
    assert(right->type == INT_V);

    if     (b <-63         ) result = intval(0);
    else if(b <  0         ) result = intval(a << -b);
    else if(b == 0         ) result = intval(a);
    else if(b < 63 && a < 0) result = intval(~(~a >> b));
    else if(b < 63         ) result = intval(a >> b);
    else if(          a < 0) result = intval(-1);
    else                     result = intval(0);

    return result ? result : nullval();
}


static VAL* OP_SHR(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);
    VAL* result = NULL;
    uint64_t a = left->value.i;
    int64_t b = right->value.i;

    assert(left->type == INT_V);
    assert(right->type == INT_V);

    if     (b <-63) result = intval(0);
    else if(b <  0) result = intval(a << -b);
    else if(b == 0) result = intval(a);
    else if(b < 63) result = intval(a >> b);
    else            result = intval(0);

    return result ? result : nullval();
}


static VAL* OP_COND(VEC* args, SYMTBL* table) {
    VAL* left = vecget(args, 0);
    VAL* right = vecget(args, 1);
    VAL* righter = vecget(args, 2);
    VAL* result = NULL;

    assert(left->type == NODE_V);
    assert(right->type == NODE_V);
    assert(righter->type == NODE_V);

    result = eval(left->value.n, table);

    if(valtrue(result)) {
        freeval(result);
        result = eval(right->value.n, table);
    }
    else {
        freeval(result);
        result = eval(righter->value.n, table);
    }

    return result;
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

MAP* binfns() {
    if(!BINFNS) {
        BINFNS = newmap();

        addfn(BINFNS, "IF"      , ".**" , IF      );
        addfn(BINFNS, "FOR"     , "....", FOR     );
        addfn(BINFNS, "LEN"     , "?"   , LEN     );
        addfn(BINFNS, "SQRT"    , "?"   , SQRT    );
        addfn(BINFNS, "PRINT"   , "*"   , PRINT   );
        addfn(BINFNS, "FUNCTION", "S."  , FUNCTION);
    }

    return BINFNS;
}


MAP* binops() {
    if(!BINOPS) {
        BINOPS = newmap();

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


VAL* fnexec(const char* key, SYMTBL* table, VEC* args) {
    VAL* fnval = mapget(binfns(), key);
    VAL* result = NULL;

    if(fnval && fnval->type == FUNCTION_V) {
        FUNC* fn = fnval->value.fn;

        result = funcexec(fn, args, table);
    }

    freevec(args);
    return result ? result : nullval();
}


VAL* opexec(const char* key, SYMTBL* table, NODE* left, NODE* right, NODE* righter) {
    VAL* fnval = mapget(binops(), key);
    VAL* result = NULL;

    if(fnval && fnval->type == FUNCTION_V) {
        FUNC* fn = fnval->value.fn;
        VEC* args = newvec();

        if(left) vecpush(args, nodeval(left));
        if(right) vecpush(args, nodeval(right));
        if(righter) vecpush(args, nodeval(righter));

        result = funcexec(fn, args, table);
        freevec(args);
    }

    return result ? result : nullval();
}
