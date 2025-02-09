#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "je_eval.h"
#include "je_oper.h"
#include "je_util.h"
#include "je_val.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_VAL* je_op_times(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = JE_ValNewFromNull();

    if     (lval->type == JE_INT_V   && rval->type == JE_INT_V )  result = JE_ValNewFromInt(lval->value.i * rval->value.i);
    else if(lval->type == JE_INT_V   && rval->type == JE_FLOAT_V) result = JE_ValNewFromFloat(lval->value.i * rval->value.f);
    else if(lval->type == JE_FLOAT_V && rval->type == JE_INT_V )  result = JE_ValNewFromFloat(lval->value.f * rval->value.i);
    else if(lval->type == JE_FLOAT_V && rval->type == JE_FLOAT_V) result = JE_ValNewFromFloat(lval->value.f * rval->value.f);

    JE_ValDelete(lval);
    JE_ValDelete(rval);

    return result;
}


JE_VAL* je_op_divby(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = JE_ValNewFromNull();

    if     (lval->type == JE_INT_V   && rval->type == JE_INT_V  && rval->value.i != 0) result = JE_ValNewFromInt(lval->value.i / rval->value.i);
    else if(lval->type == JE_INT_V   && rval->type == JE_FLOAT_V) result = JE_ValNewFromFloat(lval->value.i / rval->value.f);
    else if(lval->type == JE_FLOAT_V && rval->type == JE_INT_V  ) result = JE_ValNewFromFloat(lval->value.f / rval->value.i);
    else if(lval->type == JE_FLOAT_V && rval->type == JE_FLOAT_V) result = JE_ValNewFromFloat(lval->value.f / rval->value.f);

    JE_ValDelete(lval);
    JE_ValDelete(rval);

    return result;
}


JE_VAL* je_op_mod(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = JE_ValNewFromNull();

    if(lval->type == JE_INT_V && rval->type == JE_INT_V && rval->value.i != 0) {
        result = JE_ValNewFromInt(lval->value.i % rval->value.i);
    }

    JE_ValDelete(lval);
    JE_ValDelete(rval);

    return result;
}


JE_VAL* je_op_plus(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = JE_ValNewFromNull();

    if     (lval->type == JE_INT_V    && rval->type == JE_INT_V   ) result = JE_ValNewFromInt(lval->value.i + rval->value.i);
    else if(lval->type == JE_INT_V    && rval->type == JE_FLOAT_V ) result = JE_ValNewFromFloat(lval->value.i + rval->value.f);
    else if(lval->type == JE_FLOAT_V  && rval->type == JE_INT_V   ) result = JE_ValNewFromFloat(lval->value.f + rval->value.i);
    else if(lval->type == JE_FLOAT_V  && rval->type == JE_FLOAT_V ) result = JE_ValNewFromFloat(lval->value.f + rval->value.f);
    else if(lval->type == JE_STRING_V || rval->type == JE_STRING_V) {
        char* buf;

        asprintf(&buf, "%s%s", JE_ValToCstr(lval), JE_ValToCstr(rval));
        result = JE_ValNewFromCstr(buf);

        JE_Free(buf);
    }

    JE_ValDelete(lval);
    JE_ValDelete(rval);

    return result;
}


JE_VAL* je_op_minus(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = JE_ValNewFromNull();

    if     (lval->type == JE_INT_V   && rval->type == JE_INT_V )  result = JE_ValNewFromInt(lval->value.i - rval->value.i);
    else if(lval->type == JE_INT_V   && rval->type == JE_FLOAT_V) result = JE_ValNewFromFloat(lval->value.i - rval->value.f);
    else if(lval->type == JE_FLOAT_V && rval->type == JE_INT_V )  result = JE_ValNewFromFloat(lval->value.f - rval->value.i);
    else if(lval->type == JE_FLOAT_V && rval->type == JE_FLOAT_V) result = JE_ValNewFromFloat(lval->value.f - rval->value.f);

    JE_ValDelete(lval);
    JE_ValDelete(rval);

    return result;
}


JE_VAL* je_op_bor(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = JE_ValNewFromNull();

    if(lval->type == JE_INT_V && rval->type == JE_INT_V) {
        int64_t a = lval->value.i;
        int64_t b = rval->value.i;

        result = JE_ValNewFromInt(a | b);
    }

    JE_ValDelete(lval);
    JE_ValDelete(rval);

    return result;
}


JE_VAL* je_op_band(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = JE_ValNewFromNull();

    if(lval->type == JE_INT_V && rval->type == JE_INT_V) {
        int64_t a = lval->value.i;
        int64_t b = rval->value.i;

        result = JE_ValNewFromInt(a & b);
    }

    JE_ValDelete(lval);
    JE_ValDelete(rval);

    return result;
}


JE_VAL* je_op_bxor(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = JE_ValNewFromNull();

    if(lval->type == JE_INT_V && rval->type == JE_INT_V) {
        int64_t a = lval->value.i;
        int64_t b = rval->value.i;

        result = JE_ValNewFromInt(a ^ b);
    }

    JE_ValDelete(lval);
    JE_ValDelete(rval);

    return result;
}


JE_VAL* je_op_shl(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = JE_ValNewFromNull();

    if(lval->type == JE_INT_V && rval->type == JE_INT_V) {
        uint64_t a = lval->value.i;
        int64_t b = rval->value.i;

        if     (b <  0) return je_op_asr(lval, rval);
        else if(b == 0) result = JE_ValNewFromInt(a);
        else            result = JE_ValNewFromInt(a << b);
    }

    JE_ValDelete(lval);
    JE_ValDelete(rval);

    return result;
}


JE_VAL* je_op_asr(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = JE_ValNewFromNull();

    if(lval->type == JE_INT_V && rval->type == JE_INT_V) {
        int64_t val = lval->value.i;
        int64_t shiftby = rval->value.i;

        if(shiftby < 0) {
            JE_ValDelete(rval);
            rval = JE_ValNewFromInt(-shiftby);

            return je_op_shl(lval, rval);
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

        result = JE_ValNewFromInt(val);
    }

    JE_ValDelete(lval);
    JE_ValDelete(rval);

    return result;
}


JE_VAL* je_op_shr(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = JE_ValNewFromNull();

    if(lval->type == JE_INT_V && rval->type == JE_INT_V) {
        uint64_t val = lval->value.i;
        int64_t shiftby = rval->value.i;

        if(shiftby < 0) {
            JE_ValDelete(rval);
            rval = JE_ValNewFromInt(-shiftby);

            return je_op_shl(lval, rval);
        }
        else if(shiftby < 64) {
            val >>= shiftby;
        }
        else {
            val = 0;
        }

        result = JE_ValNewFromInt(val);
    }

    JE_ValDelete(lval);
    JE_ValDelete(rval);

    return result;
}


JE_VAL* je_op_bnot(JE_VAL* val) {
    JE_VAL* result = JE_ValNewFromNull();

    if(val->type == JE_INT_V) {
        int64_t a = val->value.i;

        result = JE_ValNewFromInt(~a);
    }

    JE_ValDelete(val);

    return result;
}


JE_VAL* je_op_lnot(JE_VAL* val) {
    JE_VAL* result = JE_ValIsTrue(val) ? JE_ValNewFromBool(0) : JE_ValNewFromBool(1);

    JE_ValDelete(val);

    return result;
}


JE_VAL* je_op_eq(JE_VAL* lval, JE_VAL* rval) {
    int cmp = JE_ValCmp(lval, rval);

    JE_ValDelete(lval);
    JE_ValDelete(rval);

    return JE_ValNewFromBool(cmp == 0);
}


JE_VAL* je_op_ne(JE_VAL* lval, JE_VAL* rval) {
    int cmp = JE_ValCmp(lval, rval);

    JE_ValDelete(lval);
    JE_ValDelete(rval);

    return JE_ValNewFromBool(cmp != 0);
}


JE_VAL* je_op_lt(JE_VAL* lval, JE_VAL* rval) {
    int cmp = JE_ValCmp(lval, rval);

    JE_ValDelete(lval);
    JE_ValDelete(rval);

    return JE_ValNewFromBool(cmp < 0);
}


JE_VAL* je_op_le(JE_VAL* lval, JE_VAL* rval) {
    int cmp = JE_ValCmp(lval, rval);

    JE_ValDelete(lval);
    JE_ValDelete(rval);

    return JE_ValNewFromBool(cmp <= 0);
}


JE_VAL* je_op_gt(JE_VAL* lval, JE_VAL* rval) {
    int cmp = JE_ValCmp(lval, rval);

    JE_ValDelete(lval);
    JE_ValDelete(rval);

    return JE_ValNewFromBool(cmp > 0);
}


JE_VAL* je_op_ge(JE_VAL* lval, JE_VAL* rval) {
    int cmp = JE_ValCmp(lval, rval);

    JE_ValDelete(lval);
    JE_ValDelete(rval);

    return JE_ValNewFromBool(cmp >= 0);
}


JE_VAL* je_op_uplus(JE_VAL* val) {
    JE_VAL* result = JE_ValNewFromNull();

    if      (val->type == JE_INT_V)  result = JE_ValNewFromInt(val->value.i);
    else if (val->type == JE_FLOAT_V) result = JE_ValNewFromFloat(val->value.f);

    JE_ValDelete(val);

    return result;
}


JE_VAL* je_op_uminus(JE_VAL* val) {
    JE_VAL* result = JE_ValNewFromNull();

    if      (val->type == JE_INT_V)  result = JE_ValNewFromInt(-val->value.i);
    else if (val->type == JE_FLOAT_V) result = JE_ValNewFromFloat(-val->value.f);

    JE_ValDelete(val);

    return result;
}


JE_VAL* je_op_pow(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = JE_ValNewFromNull();
    double a = 0.0;
    double b = 0.0;
    int isok = 1;

    switch(lval->type) {
        case JE_INT_V  : a = lval->value.i; break;
        case JE_FLOAT_V: a = lval->value.f; break;
        default     : isok = 0;
    }

    switch(rval->type) {
        case JE_INT_V  : b = rval->value.i; break;
        case JE_FLOAT_V: b = rval->value.f; break;
        default     : isok = 0;
    }

    if(isok) result = JE_ValNewFromFloat(pow(a, b));
    JE_ValDelete(lval);
    JE_ValDelete(rval);

    return result;
}


JE_VAL* je_op_lor(JE_NODE* left, JE_NODE* right, JE_SYMTBL* table) {
    JE_VAL* result = JE_EvalByNode(left, table);

    if(!JE_ValIsTrue(result)) {
        JE_ValDelete(result);
        result = JE_EvalByNode(right, table);
    }

    return result;
}


JE_VAL* je_op_land(JE_NODE* left, JE_NODE* right, JE_SYMTBL* table) {
    JE_VAL* result = JE_EvalByNode(left, table);

    if(JE_ValIsTrue(result)) {
        JE_ValDelete(result);
        result = JE_EvalByNode(right, table);
    }

    return result;
}


JE_VAL* je_op_cond(JE_NODE* test, JE_NODE* iftrue, JE_NODE* iffalse, JE_SYMTBL* table) {
    JE_VAL* val = JE_EvalByNode(test, table);
    JE_VAL* result = NULL;

    if(JE_ValIsTrue(val)) result = JE_EvalByNode(iftrue, table);
    else result = JE_EvalByNode(iffalse, table);

    JE_ValDelete(val);

    return result;
}
