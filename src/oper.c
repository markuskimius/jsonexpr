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
    else if(lvalue->type == STRING_V || rvalue->type == STRING_V) {
        char* buf;

        asprintf(&buf, "%s%s", strdecoded(lvalue), strdecoded(rvalue));
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


VALUE* op_cond(NODE* test, NODE* iftrue, NODE* iffalse, SYM_TABLE* table) {
    VALUE* value = eval(test, table);
    VALUE* result = NULL;

    if(istrue(value)) result = eval(iftrue, table);
    else result = eval(iffalse, table);

    freevalue(value);

    return result;
}
