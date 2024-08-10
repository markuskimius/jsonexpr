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

    if     (lvalue->type == INT64_V  && rvalue->type == INT64_V ) result = intvalue(lvalue->value.i64 * rvalue->value.i64);
    else if(lvalue->type == INT64_V  && rvalue->type == DOUBLE_V) result = dblvalue(lvalue->value.i64 * rvalue->value.f64);
    else if(lvalue->type == DOUBLE_V && rvalue->type == INT64_V ) result = dblvalue(lvalue->value.f64 * rvalue->value.i64);
    else if(lvalue->type == DOUBLE_V && rvalue->type == DOUBLE_V) result = dblvalue(lvalue->value.f64 * rvalue->value.f64);

    freevalue(lvalue);
    freevalue(rvalue);

    return result;
}


VALUE* op_divby(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = nullvalue();

    if     (lvalue->type == INT64_V  && rvalue->type == INT64_V  && rvalue->value.i64 != 0) result = intvalue(lvalue->value.i64 / rvalue->value.i64);
    else if(lvalue->type == INT64_V  && rvalue->type == DOUBLE_V) result = dblvalue(lvalue->value.i64 / rvalue->value.f64);
    else if(lvalue->type == DOUBLE_V && rvalue->type == INT64_V ) result = dblvalue(lvalue->value.f64 / rvalue->value.i64);
    else if(lvalue->type == DOUBLE_V && rvalue->type == DOUBLE_V) result = dblvalue(lvalue->value.f64 / rvalue->value.f64);

    freevalue(lvalue);
    freevalue(rvalue);

    return result;
}


VALUE* op_mod(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = nullvalue();

    if(lvalue->type == INT64_V  && rvalue->type == INT64_V && rvalue->value.i64 != 0) {
        result = intvalue(lvalue->value.i64 % rvalue->value.i64);
    }

    freevalue(lvalue);
    freevalue(rvalue);

    return result;
}


VALUE* op_plus(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = nullvalue();

    if     (lvalue->type == INT64_V  && rvalue->type == INT64_V ) result = intvalue(lvalue->value.i64 + rvalue->value.i64);
    else if(lvalue->type == INT64_V  && rvalue->type == DOUBLE_V) result = dblvalue(lvalue->value.i64 + rvalue->value.f64);
    else if(lvalue->type == DOUBLE_V && rvalue->type == INT64_V ) result = dblvalue(lvalue->value.f64 + rvalue->value.i64);
    else if(lvalue->type == DOUBLE_V && rvalue->type == DOUBLE_V) result = dblvalue(lvalue->value.f64 + rvalue->value.f64);
    else if(lvalue->type == STRING_V && rvalue->type == STRING_V) {
        lvalue->value.str = astrcat(lvalue->value.str, rvalue->value.str);
        result = lvalue;
    }
    else if(lvalue->type == STRING_V && rvalue->type == INT64_V ) {
        size_t len = strlen(lvalue->value.str) + IBUFSIZE;

        lvalue->value.str = realloc(lvalue->value.str, len);
        snprintf(lvalue->value.str, len, "%s%lld", lvalue->value.str, rvalue->value.i64);
        result = lvalue;
    }
    else if(lvalue->type == STRING_V && rvalue->type == DOUBLE_V) {
        size_t len = strlen(lvalue->value.str) + DBUFSIZE;

        lvalue->value.str = realloc(lvalue->value.str, len);
        snprintf(lvalue->value.str, len, "%s%lf", lvalue->value.str, rvalue->value.f64);
        result = lvalue;
    }
    else if(lvalue->type == INT64_V  && rvalue->type == STRING_V) {
        size_t len = strlen(rvalue->value.str) + IBUFSIZE;

        rvalue->value.str = realloc(rvalue->value.str, len);
        snprintf(rvalue->value.str, len, "%s%lld", rvalue->value.str, lvalue->value.i64);
        result = rvalue;
    }
    else if(lvalue->type == DOUBLE_V && rvalue->type == STRING_V) {
        size_t len = strlen(rvalue->value.str) + DBUFSIZE;

        rvalue->value.str = realloc(rvalue->value.str, len);
        snprintf(rvalue->value.str, len, "%s%lf", rvalue->value.str, lvalue->value.f64);
        result = rvalue;
    }

    if(result != lvalue) freevalue(lvalue);
    if(result != rvalue) freevalue(rvalue);

    return result;
}


VALUE* op_minus(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = nullvalue();

    if     (lvalue->type == INT64_V  && rvalue->type == INT64_V ) result = intvalue(lvalue->value.i64 - rvalue->value.i64);
    else if(lvalue->type == INT64_V  && rvalue->type == DOUBLE_V) result = dblvalue(lvalue->value.i64 - rvalue->value.f64);
    else if(lvalue->type == DOUBLE_V && rvalue->type == INT64_V ) result = dblvalue(lvalue->value.f64 - rvalue->value.i64);
    else if(lvalue->type == DOUBLE_V && rvalue->type == DOUBLE_V) result = dblvalue(lvalue->value.f64 - rvalue->value.f64);

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

    if      (value->type == INT64_V)  result = intvalue(value->value.i64);
    else if (value->type == DOUBLE_V) result = dblvalue(value->value.f64);

    freevalue(value);

    return result;
}


VALUE* op_uminus(VALUE* value) {
    VALUE* result = nullvalue();

    if      (value->type == INT64_V)  result = intvalue(-value->value.i64);
    else if (value->type == DOUBLE_V) result = dblvalue(-value->value.f64);

    freevalue(value);

    return result;
}
