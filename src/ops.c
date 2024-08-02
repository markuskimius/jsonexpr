#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ops.h"
#include "ast.h"
#include "map.h"
#include "util.h"
#include "value.h"
#include "vector.h"
#include "jsonexpr.tab.h"

#define IBUFSIZE 32
#define DBUFSIZE 320


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VALUE* op_times(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = NULL;

    if     (lvalue->type == INT64_V  && rvalue->type == INT64_V ) result = newint64(lvalue->value.i64 * rvalue->value.i64);
    else if(lvalue->type == INT64_V  && rvalue->type == DOUBLE_V) result = newdouble(lvalue->value.i64 * rvalue->value.f64);
    else if(lvalue->type == DOUBLE_V && rvalue->type == INT64_V ) result = newdouble(lvalue->value.f64 * rvalue->value.i64);
    else if(lvalue->type == DOUBLE_V && rvalue->type == DOUBLE_V) result = newdouble(lvalue->value.f64 * rvalue->value.f64);
    else                                                          result = newnull();

    freevalue(lvalue);
    freevalue(rvalue);

    return result;
}


VALUE* op_divby(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = NULL;

    if     (lvalue->type == INT64_V  && rvalue->type == INT64_V  && rvalue->value.i64 != 0) result = newint64(lvalue->value.i64 / rvalue->value.i64);
    else if(lvalue->type == INT64_V  && rvalue->type == DOUBLE_V) result = newdouble(lvalue->value.i64 / rvalue->value.f64);
    else if(lvalue->type == DOUBLE_V && rvalue->type == INT64_V ) result = newdouble(lvalue->value.f64 / rvalue->value.i64);
    else if(lvalue->type == DOUBLE_V && rvalue->type == DOUBLE_V) result = newdouble(lvalue->value.f64 / rvalue->value.f64);
    else                                                          result = newnull();

    freevalue(lvalue);
    freevalue(rvalue);

    return result;
}


VALUE* op_mod(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = NULL;

    if(lvalue->type == INT64_V  && rvalue->type == INT64_V && rvalue->value.i64 != 0) {
        result = newint64(lvalue->value.i64 % rvalue->value.i64);
    }
    else {
        result = newnull();
    }

    freevalue(lvalue);
    freevalue(rvalue);

    return result;
}


VALUE* op_plus(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = NULL;

    if     (lvalue->type == INT64_V  && rvalue->type == INT64_V ) result = newint64(lvalue->value.i64 + rvalue->value.i64);
    else if(lvalue->type == INT64_V  && rvalue->type == DOUBLE_V) result = newdouble(lvalue->value.i64 + rvalue->value.f64);
    else if(lvalue->type == DOUBLE_V && rvalue->type == INT64_V ) result = newdouble(lvalue->value.f64 + rvalue->value.i64);
    else if(lvalue->type == DOUBLE_V && rvalue->type == DOUBLE_V) result = newdouble(lvalue->value.f64 + rvalue->value.f64);
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
    else {
        result = newnull();
    }

    if(result != lvalue) freevalue(lvalue);
    if(result != rvalue) freevalue(rvalue);

    return result;
}


VALUE* op_minus(VALUE* lvalue, VALUE* rvalue) {
    VALUE* result = NULL;

    if     (lvalue->type == INT64_V  && rvalue->type == INT64_V ) result = newint64(lvalue->value.i64 - rvalue->value.i64);
    else if(lvalue->type == INT64_V  && rvalue->type == DOUBLE_V) result = newdouble(lvalue->value.i64 - rvalue->value.f64);
    else if(lvalue->type == DOUBLE_V && rvalue->type == INT64_V ) result = newdouble(lvalue->value.f64 - rvalue->value.i64);
    else if(lvalue->type == DOUBLE_V && rvalue->type == DOUBLE_V) result = newdouble(lvalue->value.f64 - rvalue->value.f64);
    else                                                          result = newnull();

    freevalue(lvalue);
    freevalue(rvalue);

    return result;
}


VALUE* op_uplus(VALUE* value) {
    if      (value->type == INT64_V)  value->value.i64 = value->value.i64;
    else if (value->type == DOUBLE_V) value->value.f64 = value->value.f64;
    else {
        value->type = NULL_V;
        value->value.i64 = 0;
    }

    return value;
}


VALUE* op_uminus(VALUE* value) {
    if      (value->type == INT64_V)  value->value.i64 = -value->value.i64;
    else if (value->type == DOUBLE_V) value->value.f64 = -value->value.f64;
    else {
        value->type = NULL_V;
        value->value.i64 = 0;
    }

    return value;
}
