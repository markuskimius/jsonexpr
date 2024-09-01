#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"
#include "eval.h"
#include "node.h"
#include "oper.h"
#include "util.h"
#include "parse.h"
#include "value.h"
#include "vec.h"

#define IBUFSIZE 32
#define DBUFSIZE 320


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VALUE* op_times(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = nullvalue();

    if     (lvalue->type == INT_V   && rvalue->type == INT_V )  result = intvalue(lvalue->value.i * rvalue->value.i);
    else if(lvalue->type == INT_V   && rvalue->type == FLOAT_V) result = dblvalue(lvalue->value.i * rvalue->value.f);
    else if(lvalue->type == FLOAT_V && rvalue->type == INT_V )  result = dblvalue(lvalue->value.f * rvalue->value.i);
    else if(lvalue->type == FLOAT_V && rvalue->type == FLOAT_V) result = dblvalue(lvalue->value.f * rvalue->value.f);

    freevalue(lvalue);
    freevalue(rvalue);

    return result;
}


VALUE* op_divby(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = nullvalue();

    if     (lvalue->type == INT_V   && rvalue->type == INT_V  && rvalue->value.i != 0) result = intvalue(lvalue->value.i / rvalue->value.i);
    else if(lvalue->type == INT_V   && rvalue->type == FLOAT_V) result = dblvalue(lvalue->value.i / rvalue->value.f);
    else if(lvalue->type == FLOAT_V && rvalue->type == INT_V  ) result = dblvalue(lvalue->value.f / rvalue->value.i);
    else if(lvalue->type == FLOAT_V && rvalue->type == FLOAT_V) result = dblvalue(lvalue->value.f / rvalue->value.f);

    freevalue(lvalue);
    freevalue(rvalue);

    return result;
}


VALUE* op_mod(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = nullvalue();

    if(lvalue->type == INT_V && rvalue->type == INT_V && rvalue->value.i != 0) {
        result = intvalue(lvalue->value.i % rvalue->value.i);
    }

    freevalue(lvalue);
    freevalue(rvalue);

    return result;
}


VALUE* op_plus(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = nullvalue();

    if     (lvalue->type == INT_V    && rvalue->type == INT_V   ) result = intvalue(lvalue->value.i + rvalue->value.i);
    else if(lvalue->type == INT_V    && rvalue->type == FLOAT_V ) result = dblvalue(lvalue->value.i + rvalue->value.f);
    else if(lvalue->type == FLOAT_V  && rvalue->type == INT_V   ) result = dblvalue(lvalue->value.f + rvalue->value.i);
    else if(lvalue->type == FLOAT_V  && rvalue->type == FLOAT_V ) result = dblvalue(lvalue->value.f + rvalue->value.f);
    else if(lvalue->type == STRING_V || rvalue->type == STRING_V) {
        char* buf;

        asprintf(&buf, "%s%s", valuestr(lvalue), valuestr(rvalue));
        result = strvalue(buf);
    }

    freevalue(lvalue);
    freevalue(rvalue);

    return result;
}


VALUE* op_minus(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = nullvalue();

    if     (lvalue->type == INT_V   && rvalue->type == INT_V )  result = intvalue(lvalue->value.i - rvalue->value.i);
    else if(lvalue->type == INT_V   && rvalue->type == FLOAT_V) result = dblvalue(lvalue->value.i - rvalue->value.f);
    else if(lvalue->type == FLOAT_V && rvalue->type == INT_V )  result = dblvalue(lvalue->value.f - rvalue->value.i);
    else if(lvalue->type == FLOAT_V && rvalue->type == FLOAT_V) result = dblvalue(lvalue->value.f - rvalue->value.f);

    freevalue(lvalue);
    freevalue(rvalue);

    return result;
}


VALUE* op_bor(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = nullvalue();

    if(lvalue->type == INT_V && rvalue->type == INT_V) {
        int64_t a = lvalue->value.i;
        int64_t b = rvalue->value.i;

        result = intvalue(a | b);
    }

    freevalue(lvalue);
    freevalue(rvalue);

    return result;
}


VALUE* op_band(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = nullvalue();

    if(lvalue->type == INT_V && rvalue->type == INT_V) {
        int64_t a = lvalue->value.i;
        int64_t b = rvalue->value.i;

        result = intvalue(a & b);
    }

    freevalue(lvalue);
    freevalue(rvalue);

    return result;
}


VALUE* op_bxor(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = nullvalue();

    if(lvalue->type == INT_V && rvalue->type == INT_V) {
        int64_t a = lvalue->value.i;
        int64_t b = rvalue->value.i;

        result = intvalue(a ^ b);
    }

    freevalue(lvalue);
    freevalue(rvalue);

    return result;
}


VALUE* op_shl(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = nullvalue();

    if(lvalue->type == INT_V && rvalue->type == INT_V) {
        uint64_t a = lvalue->value.i;
        int64_t b = rvalue->value.i;

        if     (b <  0) return op_asr(lvalue, rvalue);
        else if(b == 0) result = intvalue(a);
        else            result = intvalue(a << b);
    }

    freevalue(lvalue);
    freevalue(rvalue);

    return result;
}


VALUE* op_asr(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = nullvalue();

    if(lvalue->type == INT_V && rvalue->type == INT_V) {
        int64_t value = lvalue->value.i;
        int64_t shiftby = rvalue->value.i;

        if(shiftby < 0) {
            freevalue(rvalue);
            rvalue = intvalue(-shiftby);

            return op_shl(lvalue, rvalue);
        }
        else if(shiftby < 64 && value < 0) {
            value = ~value;
            value >>= shiftby;
            value = ~value;
        }
        else if(shiftby < 64) {
            value >>= shiftby;
        }
        else {
            value = 0;
        }

        result = intvalue(value);
    }

    freevalue(lvalue);
    freevalue(rvalue);

    return result;
}


VALUE* op_shr(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = nullvalue();

    if(lvalue->type == INT_V && rvalue->type == INT_V) {
        uint64_t value = lvalue->value.i;
        int64_t shiftby = rvalue->value.i;

        if(shiftby < 0) {
            freevalue(rvalue);
            rvalue = intvalue(-shiftby);

            return op_shl(lvalue, rvalue);
        }
        else if(shiftby < 64) {
            value >>= shiftby;
        }
        else {
            value = 0;
        }

        result = intvalue(value);
    }

    freevalue(lvalue);
    freevalue(rvalue);

    return result;
}


VALUE* op_bnot(VALUE* value) {
    VALUE* result = nullvalue();

    if(value->type == INT_V) {
        int64_t a = value->value.i;

        result = intvalue(~a);
    }

    freevalue(value);

    return result;
}


VALUE* op_lnot(VALUE* value) {
    VALUE* result = valuetrue(value) ? boolvalue(0) : boolvalue(1);

    freevalue(value);

    return result;
}


VALUE* op_eq(VALUE* lvalue, VALUE* rvalue) {
    int cmp = valuecmp(lvalue, rvalue);

    freevalue(lvalue);
    freevalue(rvalue);

    return boolvalue(cmp == 0);
}


VALUE* op_ne(VALUE* lvalue, VALUE* rvalue) {
    int cmp = valuecmp(lvalue, rvalue);

    freevalue(lvalue);
    freevalue(rvalue);

    return boolvalue(cmp != 0);
}


VALUE* op_lt(VALUE* lvalue, VALUE* rvalue) {
    int cmp = valuecmp(lvalue, rvalue);

    freevalue(lvalue);
    freevalue(rvalue);

    return boolvalue(cmp < 0);
}


VALUE* op_le(VALUE* lvalue, VALUE* rvalue) {
    int cmp = valuecmp(lvalue, rvalue);

    freevalue(lvalue);
    freevalue(rvalue);

    return boolvalue(cmp <= 0);
}


VALUE* op_gt(VALUE* lvalue, VALUE* rvalue) {
    int cmp = valuecmp(lvalue, rvalue);

    freevalue(lvalue);
    freevalue(rvalue);

    return boolvalue(cmp > 0);
}


VALUE* op_ge(VALUE* lvalue, VALUE* rvalue) {
    int cmp = valuecmp(lvalue, rvalue);

    freevalue(lvalue);
    freevalue(rvalue);

    return boolvalue(cmp >= 0);
}


VALUE* op_uplus(VALUE* value) {
    VALUE* result = nullvalue();

    if      (value->type == INT_V)  result = intvalue(value->value.i);
    else if (value->type == FLOAT_V) result = dblvalue(value->value.f);

    freevalue(value);

    return result;
}


VALUE* op_uminus(VALUE* value) {
    VALUE* result = nullvalue();

    if      (value->type == INT_V)  result = intvalue(-value->value.i);
    else if (value->type == FLOAT_V) result = dblvalue(-value->value.f);

    freevalue(value);

    return result;
}


VALUE* op_pow(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = nullvalue();
    double a = 0.0;
    double b = 0.0;
    int isok = 1;

    switch(lvalue->type) {
        case INT_V  : a = lvalue->value.i; break;
        case FLOAT_V: a = lvalue->value.f; break;
        default     : isok = 0;
    }

    switch(rvalue->type) {
        case INT_V  : b = rvalue->value.i; break;
        case FLOAT_V: b = rvalue->value.f; break;
        default     : isok = 0;
    }

    if(isok) result = dblvalue(pow(a, b));
    freevalue(lvalue);
    freevalue(rvalue);

    return result;
}


VALUE* op_lor(NODE* left, NODE* right, SYMTBL* table) {
    VALUE* result = eval(left, table);

    if(!valuetrue(result)) {
        freevalue(result);
        result = eval(right, table);
    }

    return result;
}


VALUE* op_land(NODE* left, NODE* right, SYMTBL* table) {
    VALUE* result = eval(left, table);

    if(valuetrue(result)) {
        freevalue(result);
        result = eval(right, table);
    }

    return result;
}


VALUE* op_cond(NODE* test, NODE* iftrue, NODE* iffalse, SYMTBL* table) {
    VALUE* value = eval(test, table);
    VALUE* result = NULL;

    if(valuetrue(value)) result = eval(iftrue, table);
    else result = eval(iffalse, table);

    freevalue(value);

    return result;
}
