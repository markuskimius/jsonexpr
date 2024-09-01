#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eval.h"
#include "oper.h"
#include "val.h"

#define IBUFSIZE 32
#define DBUFSIZE 320


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VAL* op_times(VAL* lval, VAL* rval) {
    VAL* result = nullval();

    if     (lval->type == INT_V   && rval->type == INT_V )  result = intval(lval->value.i * rval->value.i);
    else if(lval->type == INT_V   && rval->type == FLOAT_V) result = dblval(lval->value.i * rval->value.f);
    else if(lval->type == FLOAT_V && rval->type == INT_V )  result = dblval(lval->value.f * rval->value.i);
    else if(lval->type == FLOAT_V && rval->type == FLOAT_V) result = dblval(lval->value.f * rval->value.f);

    freeval(lval);
    freeval(rval);

    return result;
}


VAL* op_divby(VAL* lval, VAL* rval) {
    VAL* result = nullval();

    if     (lval->type == INT_V   && rval->type == INT_V  && rval->value.i != 0) result = intval(lval->value.i / rval->value.i);
    else if(lval->type == INT_V   && rval->type == FLOAT_V) result = dblval(lval->value.i / rval->value.f);
    else if(lval->type == FLOAT_V && rval->type == INT_V  ) result = dblval(lval->value.f / rval->value.i);
    else if(lval->type == FLOAT_V && rval->type == FLOAT_V) result = dblval(lval->value.f / rval->value.f);

    freeval(lval);
    freeval(rval);

    return result;
}


VAL* op_mod(VAL* lval, VAL* rval) {
    VAL* result = nullval();

    if(lval->type == INT_V && rval->type == INT_V && rval->value.i != 0) {
        result = intval(lval->value.i % rval->value.i);
    }

    freeval(lval);
    freeval(rval);

    return result;
}


VAL* op_plus(VAL* lval, VAL* rval) {
    VAL* result = nullval();

    if     (lval->type == INT_V    && rval->type == INT_V   ) result = intval(lval->value.i + rval->value.i);
    else if(lval->type == INT_V    && rval->type == FLOAT_V ) result = dblval(lval->value.i + rval->value.f);
    else if(lval->type == FLOAT_V  && rval->type == INT_V   ) result = dblval(lval->value.f + rval->value.i);
    else if(lval->type == FLOAT_V  && rval->type == FLOAT_V ) result = dblval(lval->value.f + rval->value.f);
    else if(lval->type == STRING_V || rval->type == STRING_V) {
        char* buf;

        asprintf(&buf, "%s%s", valstr(lval), valstr(rval));
        result = strval(buf);
    }

    freeval(lval);
    freeval(rval);

    return result;
}


VAL* op_minus(VAL* lval, VAL* rval) {
    VAL* result = nullval();

    if     (lval->type == INT_V   && rval->type == INT_V )  result = intval(lval->value.i - rval->value.i);
    else if(lval->type == INT_V   && rval->type == FLOAT_V) result = dblval(lval->value.i - rval->value.f);
    else if(lval->type == FLOAT_V && rval->type == INT_V )  result = dblval(lval->value.f - rval->value.i);
    else if(lval->type == FLOAT_V && rval->type == FLOAT_V) result = dblval(lval->value.f - rval->value.f);

    freeval(lval);
    freeval(rval);

    return result;
}


VAL* op_bor(VAL* lval, VAL* rval) {
    VAL* result = nullval();

    if(lval->type == INT_V && rval->type == INT_V) {
        int64_t a = lval->value.i;
        int64_t b = rval->value.i;

        result = intval(a | b);
    }

    freeval(lval);
    freeval(rval);

    return result;
}


VAL* op_band(VAL* lval, VAL* rval) {
    VAL* result = nullval();

    if(lval->type == INT_V && rval->type == INT_V) {
        int64_t a = lval->value.i;
        int64_t b = rval->value.i;

        result = intval(a & b);
    }

    freeval(lval);
    freeval(rval);

    return result;
}


VAL* op_bxor(VAL* lval, VAL* rval) {
    VAL* result = nullval();

    if(lval->type == INT_V && rval->type == INT_V) {
        int64_t a = lval->value.i;
        int64_t b = rval->value.i;

        result = intval(a ^ b);
    }

    freeval(lval);
    freeval(rval);

    return result;
}


VAL* op_shl(VAL* lval, VAL* rval) {
    VAL* result = nullval();

    if(lval->type == INT_V && rval->type == INT_V) {
        uint64_t a = lval->value.i;
        int64_t b = rval->value.i;

        if     (b <  0) return op_asr(lval, rval);
        else if(b == 0) result = intval(a);
        else            result = intval(a << b);
    }

    freeval(lval);
    freeval(rval);

    return result;
}


VAL* op_asr(VAL* lval, VAL* rval) {
    VAL* result = nullval();

    if(lval->type == INT_V && rval->type == INT_V) {
        int64_t val = lval->value.i;
        int64_t shiftby = rval->value.i;

        if(shiftby < 0) {
            freeval(rval);
            rval = intval(-shiftby);

            return op_shl(lval, rval);
        }
        else if(shiftby < 64 && val < 0) {
            val = ~val;
            val >>= shiftby;
            val = ~val;
        }
        else if(shiftby < 64) {
            val >>= shiftby;
        }
        else {
            val = 0;
        }

        result = intval(val);
    }

    freeval(lval);
    freeval(rval);

    return result;
}


VAL* op_shr(VAL* lval, VAL* rval) {
    VAL* result = nullval();

    if(lval->type == INT_V && rval->type == INT_V) {
        uint64_t val = lval->value.i;
        int64_t shiftby = rval->value.i;

        if(shiftby < 0) {
            freeval(rval);
            rval = intval(-shiftby);

            return op_shl(lval, rval);
        }
        else if(shiftby < 64) {
            val >>= shiftby;
        }
        else {
            val = 0;
        }

        result = intval(val);
    }

    freeval(lval);
    freeval(rval);

    return result;
}


VAL* op_bnot(VAL* val) {
    VAL* result = nullval();

    if(val->type == INT_V) {
        int64_t a = val->value.i;

        result = intval(~a);
    }

    freeval(val);

    return result;
}


VAL* op_lnot(VAL* val) {
    VAL* result = valtrue(val) ? boolval(0) : boolval(1);

    freeval(val);

    return result;
}


VAL* op_eq(VAL* lval, VAL* rval) {
    int cmp = valcmp(lval, rval);

    freeval(lval);
    freeval(rval);

    return boolval(cmp == 0);
}


VAL* op_ne(VAL* lval, VAL* rval) {
    int cmp = valcmp(lval, rval);

    freeval(lval);
    freeval(rval);

    return boolval(cmp != 0);
}


VAL* op_lt(VAL* lval, VAL* rval) {
    int cmp = valcmp(lval, rval);

    freeval(lval);
    freeval(rval);

    return boolval(cmp < 0);
}


VAL* op_le(VAL* lval, VAL* rval) {
    int cmp = valcmp(lval, rval);

    freeval(lval);
    freeval(rval);

    return boolval(cmp <= 0);
}


VAL* op_gt(VAL* lval, VAL* rval) {
    int cmp = valcmp(lval, rval);

    freeval(lval);
    freeval(rval);

    return boolval(cmp > 0);
}


VAL* op_ge(VAL* lval, VAL* rval) {
    int cmp = valcmp(lval, rval);

    freeval(lval);
    freeval(rval);

    return boolval(cmp >= 0);
}


VAL* op_uplus(VAL* val) {
    VAL* result = nullval();

    if      (val->type == INT_V)  result = intval(val->value.i);
    else if (val->type == FLOAT_V) result = dblval(val->value.f);

    freeval(val);

    return result;
}


VAL* op_uminus(VAL* val) {
    VAL* result = nullval();

    if      (val->type == INT_V)  result = intval(-val->value.i);
    else if (val->type == FLOAT_V) result = dblval(-val->value.f);

    freeval(val);

    return result;
}


VAL* op_pow(VAL* lval, VAL* rval) {
    VAL* result = nullval();
    double a = 0.0;
    double b = 0.0;
    int isok = 1;

    switch(lval->type) {
        case INT_V  : a = lval->value.i; break;
        case FLOAT_V: a = lval->value.f; break;
        default     : isok = 0;
    }

    switch(rval->type) {
        case INT_V  : b = rval->value.i; break;
        case FLOAT_V: b = rval->value.f; break;
        default     : isok = 0;
    }

    if(isok) result = dblval(pow(a, b));
    freeval(lval);
    freeval(rval);

    return result;
}


VAL* op_lor(NODE* left, NODE* right, SYMTBL* table) {
    VAL* result = eval(left, table);

    if(!valtrue(result)) {
        freeval(result);
        result = eval(right, table);
    }

    return result;
}


VAL* op_land(NODE* left, NODE* right, SYMTBL* table) {
    VAL* result = eval(left, table);

    if(valtrue(result)) {
        freeval(result);
        result = eval(right, table);
    }

    return result;
}


VAL* op_cond(NODE* test, NODE* iftrue, NODE* iffalse, SYMTBL* table) {
    VAL* val = eval(test, table);
    VAL* result = NULL;

    if(valtrue(val)) result = eval(iftrue, table);
    else result = eval(iffalse, table);

    freeval(val);

    return result;
}
