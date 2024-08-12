#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"
#include "node.h"
#include "oper.h"
#include "util.h"
#include "parse.h"
#include "value.h"
#include "vector.h"

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
    else if(lvalue->type == STRING_V && rvalue->type == STRING_V) {
        lvalue->value.s = astrcat(lvalue->value.s, rvalue->value.s);
        result = lvalue;
    }
    else if(lvalue->type == STRING_V && rvalue->type == INT_V ) {
        size_t len = strlen(lvalue->value.s) + IBUFSIZE;

        lvalue->value.s = realloc(lvalue->value.s, len);
        snprintf(lvalue->value.s, len, "%s%ld", lvalue->value.s, rvalue->value.i);
        result = lvalue;
    }
    else if(lvalue->type == STRING_V && rvalue->type == FLOAT_V) {
        size_t len = strlen(lvalue->value.s) + DBUFSIZE;

        lvalue->value.s = realloc(lvalue->value.s, len);
        snprintf(lvalue->value.s, len, "%s%lf", lvalue->value.s, rvalue->value.f);
        result = lvalue;
    }
    else if(lvalue->type == INT_V && rvalue->type == STRING_V) {
        size_t len = strlen(rvalue->value.s) + IBUFSIZE;

        rvalue->value.s = realloc(rvalue->value.s, len);
        snprintf(rvalue->value.s, len, "%s%ld", rvalue->value.s, lvalue->value.i);
        result = rvalue;
    }
    else if(lvalue->type == FLOAT_V && rvalue->type == STRING_V) {
        size_t len = strlen(rvalue->value.s) + DBUFSIZE;

        rvalue->value.s = realloc(rvalue->value.s, len);
        snprintf(rvalue->value.s, len, "%s%lf", rvalue->value.s, lvalue->value.f);
        result = rvalue;
    }

    if(result != lvalue) freevalue(lvalue);
    if(result != rvalue) freevalue(rvalue);

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


VALUE* op_eq(VALUE* lvalue, VALUE* rvalue) {
    int cmp = cmpvalue(lvalue, rvalue);

    freevalue(lvalue);
    freevalue(rvalue);

    return boolvalue(cmp == 0);
}


VALUE* op_ne(VALUE* lvalue, VALUE* rvalue) {
    int cmp = cmpvalue(lvalue, rvalue);

    freevalue(lvalue);
    freevalue(rvalue);

    return boolvalue(cmp != 0);
}


VALUE* op_lt(VALUE* lvalue, VALUE* rvalue) {
    int cmp = cmpvalue(lvalue, rvalue);

    freevalue(lvalue);
    freevalue(rvalue);

    return boolvalue(cmp < 0);
}


VALUE* op_le(VALUE* lvalue, VALUE* rvalue) {
    int cmp = cmpvalue(lvalue, rvalue);

    freevalue(lvalue);
    freevalue(rvalue);

    return boolvalue(cmp <= 0);
}


VALUE* op_gt(VALUE* lvalue, VALUE* rvalue) {
    int cmp = cmpvalue(lvalue, rvalue);

    freevalue(lvalue);
    freevalue(rvalue);

    return boolvalue(cmp > 0);
}


VALUE* op_ge(VALUE* lvalue, VALUE* rvalue) {
    int cmp = cmpvalue(lvalue, rvalue);

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
