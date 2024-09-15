#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "je_eval.h"
#include "je_oper.h"
#include "je_val.h"

#define IBUFSIZE 32
#define DBUFSIZE 320


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_VAL* je_op_times(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = je_nullval();

    if     (lval->type == JE_INT_V   && rval->type == JE_INT_V )  result = je_intval(lval->value.i * rval->value.i);
    else if(lval->type == JE_INT_V   && rval->type == JE_FLOAT_V) result = je_dblval(lval->value.i * rval->value.f);
    else if(lval->type == JE_FLOAT_V && rval->type == JE_INT_V )  result = je_dblval(lval->value.f * rval->value.i);
    else if(lval->type == JE_FLOAT_V && rval->type == JE_FLOAT_V) result = je_dblval(lval->value.f * rval->value.f);

    je_freeval(lval);
    je_freeval(rval);

    return result;
}


JE_VAL* je_op_divby(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = je_nullval();

    if     (lval->type == JE_INT_V   && rval->type == JE_INT_V  && rval->value.i != 0) result = je_intval(lval->value.i / rval->value.i);
    else if(lval->type == JE_INT_V   && rval->type == JE_FLOAT_V) result = je_dblval(lval->value.i / rval->value.f);
    else if(lval->type == JE_FLOAT_V && rval->type == JE_INT_V  ) result = je_dblval(lval->value.f / rval->value.i);
    else if(lval->type == JE_FLOAT_V && rval->type == JE_FLOAT_V) result = je_dblval(lval->value.f / rval->value.f);

    je_freeval(lval);
    je_freeval(rval);

    return result;
}


JE_VAL* je_op_mod(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = je_nullval();

    if(lval->type == JE_INT_V && rval->type == JE_INT_V && rval->value.i != 0) {
        result = je_intval(lval->value.i % rval->value.i);
    }

    je_freeval(lval);
    je_freeval(rval);

    return result;
}


JE_VAL* je_op_plus(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = je_nullval();

    if     (lval->type == JE_INT_V    && rval->type == JE_INT_V   ) result = je_intval(lval->value.i + rval->value.i);
    else if(lval->type == JE_INT_V    && rval->type == JE_FLOAT_V ) result = je_dblval(lval->value.i + rval->value.f);
    else if(lval->type == JE_FLOAT_V  && rval->type == JE_INT_V   ) result = je_dblval(lval->value.f + rval->value.i);
    else if(lval->type == JE_FLOAT_V  && rval->type == JE_FLOAT_V ) result = je_dblval(lval->value.f + rval->value.f);
    else if(lval->type == JE_STRING_V || rval->type == JE_STRING_V) {
        char* buf;

        asprintf(&buf, "%s%s", je_valstr(lval), je_valstr(rval));
        result = je_strval(buf);

        free(buf);
    }

    je_freeval(lval);
    je_freeval(rval);

    return result;
}


JE_VAL* je_op_minus(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = je_nullval();

    if     (lval->type == JE_INT_V   && rval->type == JE_INT_V )  result = je_intval(lval->value.i - rval->value.i);
    else if(lval->type == JE_INT_V   && rval->type == JE_FLOAT_V) result = je_dblval(lval->value.i - rval->value.f);
    else if(lval->type == JE_FLOAT_V && rval->type == JE_INT_V )  result = je_dblval(lval->value.f - rval->value.i);
    else if(lval->type == JE_FLOAT_V && rval->type == JE_FLOAT_V) result = je_dblval(lval->value.f - rval->value.f);

    je_freeval(lval);
    je_freeval(rval);

    return result;
}


JE_VAL* je_op_bor(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = je_nullval();

    if(lval->type == JE_INT_V && rval->type == JE_INT_V) {
        int64_t a = lval->value.i;
        int64_t b = rval->value.i;

        result = je_intval(a | b);
    }

    je_freeval(lval);
    je_freeval(rval);

    return result;
}


JE_VAL* je_op_band(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = je_nullval();

    if(lval->type == JE_INT_V && rval->type == JE_INT_V) {
        int64_t a = lval->value.i;
        int64_t b = rval->value.i;

        result = je_intval(a & b);
    }

    je_freeval(lval);
    je_freeval(rval);

    return result;
}


JE_VAL* je_op_bxor(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = je_nullval();

    if(lval->type == JE_INT_V && rval->type == JE_INT_V) {
        int64_t a = lval->value.i;
        int64_t b = rval->value.i;

        result = je_intval(a ^ b);
    }

    je_freeval(lval);
    je_freeval(rval);

    return result;
}


JE_VAL* je_op_shl(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = je_nullval();

    if(lval->type == JE_INT_V && rval->type == JE_INT_V) {
        uint64_t a = lval->value.i;
        int64_t b = rval->value.i;

        if     (b <  0) return je_op_asr(lval, rval);
        else if(b == 0) result = je_intval(a);
        else            result = je_intval(a << b);
    }

    je_freeval(lval);
    je_freeval(rval);

    return result;
}


JE_VAL* je_op_asr(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = je_nullval();

    if(lval->type == JE_INT_V && rval->type == JE_INT_V) {
        int64_t val = lval->value.i;
        int64_t shiftby = rval->value.i;

        if(shiftby < 0) {
            je_freeval(rval);
            rval = je_intval(-shiftby);

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

        result = je_intval(val);
    }

    je_freeval(lval);
    je_freeval(rval);

    return result;
}


JE_VAL* je_op_shr(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = je_nullval();

    if(lval->type == JE_INT_V && rval->type == JE_INT_V) {
        uint64_t val = lval->value.i;
        int64_t shiftby = rval->value.i;

        if(shiftby < 0) {
            je_freeval(rval);
            rval = je_intval(-shiftby);

            return je_op_shl(lval, rval);
        }
        else if(shiftby < 64) {
            val >>= shiftby;
        }
        else {
            val = 0;
        }

        result = je_intval(val);
    }

    je_freeval(lval);
    je_freeval(rval);

    return result;
}


JE_VAL* je_op_bnot(JE_VAL* val) {
    JE_VAL* result = je_nullval();

    if(val->type == JE_INT_V) {
        int64_t a = val->value.i;

        result = je_intval(~a);
    }

    je_freeval(val);

    return result;
}


JE_VAL* je_op_lnot(JE_VAL* val) {
    JE_VAL* result = je_valtrue(val) ? je_boolval(0) : je_boolval(1);

    je_freeval(val);

    return result;
}


JE_VAL* je_op_eq(JE_VAL* lval, JE_VAL* rval) {
    int cmp = je_valcmp(lval, rval);

    je_freeval(lval);
    je_freeval(rval);

    return je_boolval(cmp == 0);
}


JE_VAL* je_op_ne(JE_VAL* lval, JE_VAL* rval) {
    int cmp = je_valcmp(lval, rval);

    je_freeval(lval);
    je_freeval(rval);

    return je_boolval(cmp != 0);
}


JE_VAL* je_op_lt(JE_VAL* lval, JE_VAL* rval) {
    int cmp = je_valcmp(lval, rval);

    je_freeval(lval);
    je_freeval(rval);

    return je_boolval(cmp < 0);
}


JE_VAL* je_op_le(JE_VAL* lval, JE_VAL* rval) {
    int cmp = je_valcmp(lval, rval);

    je_freeval(lval);
    je_freeval(rval);

    return je_boolval(cmp <= 0);
}


JE_VAL* je_op_gt(JE_VAL* lval, JE_VAL* rval) {
    int cmp = je_valcmp(lval, rval);

    je_freeval(lval);
    je_freeval(rval);

    return je_boolval(cmp > 0);
}


JE_VAL* je_op_ge(JE_VAL* lval, JE_VAL* rval) {
    int cmp = je_valcmp(lval, rval);

    je_freeval(lval);
    je_freeval(rval);

    return je_boolval(cmp >= 0);
}


JE_VAL* je_op_uplus(JE_VAL* val) {
    JE_VAL* result = je_nullval();

    if      (val->type == JE_INT_V)  result = je_intval(val->value.i);
    else if (val->type == JE_FLOAT_V) result = je_dblval(val->value.f);

    je_freeval(val);

    return result;
}


JE_VAL* je_op_uminus(JE_VAL* val) {
    JE_VAL* result = je_nullval();

    if      (val->type == JE_INT_V)  result = je_intval(-val->value.i);
    else if (val->type == JE_FLOAT_V) result = je_dblval(-val->value.f);

    je_freeval(val);

    return result;
}


JE_VAL* je_op_pow(JE_VAL* lval, JE_VAL* rval) {
    JE_VAL* result = je_nullval();
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

    if(isok) result = je_dblval(pow(a, b));
    je_freeval(lval);
    je_freeval(rval);

    return result;
}


JE_VAL* je_op_lor(JE_NODE* left, JE_NODE* right, JE_SYMTBL* table) {
    JE_VAL* result = je_eval(left, table);

    if(!je_valtrue(result)) {
        je_freeval(result);
        result = je_eval(right, table);
    }

    return result;
}


JE_VAL* je_op_land(JE_NODE* left, JE_NODE* right, JE_SYMTBL* table) {
    JE_VAL* result = je_eval(left, table);

    if(je_valtrue(result)) {
        je_freeval(result);
        result = je_eval(right, table);
    }

    return result;
}


JE_VAL* je_op_cond(JE_NODE* test, JE_NODE* iftrue, JE_NODE* iffalse, JE_SYMTBL* table) {
    JE_VAL* val = je_eval(test, table);
    JE_VAL* result = NULL;

    if(je_valtrue(val)) result = je_eval(iftrue, table);
    else result = je_eval(iffalse, table);

    je_freeval(val);

    return result;
}
