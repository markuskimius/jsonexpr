#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func.h"
#include "map.h"
#include "util.h"
#include "throw.h"
#include "ufunc.h"
#include "value.h"
#include "vector.h"


/* ***************************************************************************
* GLOBALS
*/

VALUE* null = NULL;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VALUE* nullvalue() {
    if(!null) {
        null = calloc(1, sizeof(VALUE));

        null->type = NULL_V;
        null->count = 1;
    }

    return null;
}


VALUE* boolvalue(int64_t i64) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = BOOL_V;
    value->count = 1;
    value->value.i64 = i64 ? 1 : 0;

    return value;
}


VALUE* intvalue(int64_t i64) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = INT64_V;
    value->count = 1;
    value->value.i64 = i64;

    return value;
}


VALUE* dblvalue(double f64) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = DOUBLE_V;
    value->count = 1;
    value->value.f64 = f64;

    return value;
}


VALUE* strvalue(const char* str) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = STRING_V;
    value->count = 1;
    value->value.str = strdup(str);

    return value;
}


VALUE* arrvalue(VEC* vec) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = ARRAY_V;
    value->count = 1;
    value->value.vec = vec ? vec : newvec();

    return value;
}


VALUE* objvalue(MAP* map) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = OBJECT_V;
    value->count = 1;
    value->value.map = map ? map : newmap();

    return value;
}


VALUE* bfnvalue(FUNC* func) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = BUILTIN_V;
    value->count = 1;
    value->value.bfn = func;

    return value;
}


VALUE* ufnvalue(UFUNC* ufunc) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = USERFUNC_V;
    value->count = 1;
    value->value.ufn = ufunc;

    return value;
}


VALUE* nodevalue(NODE* node) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = NODE_V;
    value->count = 1;
    value->value.node = node;

    return value;
}


VALUE* dupvalue(VALUE* value) {
    value->count++;

    return value;
}


void freevalue(VALUE* value) {
    if(value && --value->count == 0) {
        switch(value->type) {
            case NULL_V     : /* Do not free null */ return;
            case BOOL_V     : break;
            case INT64_V    : break;
            case DOUBLE_V   : break;
            case STRING_V   : free(value->value.str); break;
            case ARRAY_V    : freevec(value->value.vec); break;
            case OBJECT_V   : freemap(value->value.map); break;
            case BUILTIN_V  : freefunc(value->value.bfn); break;
            case USERFUNC_V : freeufunc(value->value.ufn); break;
            case NODE_V     : break;
            default         : die("Invalid value type '%c' (%d)\n", value->type, value->type); break;
        }

        if(value->astrdecoded) free(value->astrdecoded);
        if(value->astrencoded) free(value->astrencoded);
        free(value);
    }
}


char* strdecoded(VALUE* value) {
    if(value) {
        /* Reset the previous representation */
        if(value->astrdecoded && value->type != NULL_V) {
            free(value->astrdecoded);
            value->astrdecoded = NULL;
        }

        /* Generate new representation */
        switch(value->type) {
            case NULL_V     : value->astrdecoded = strdup("null"); break;
            case BOOL_V     : value->astrdecoded = value->value.i64 ? strdup("true") : strdup("false"); break;
            case INT64_V    : value->astrdecoded = astri64(value->value.i64); break;
            case DOUBLE_V   : value->astrdecoded = astrf64(value->value.f64); break;
            case STRING_V   : value->astrdecoded = strdup(value->value.str); break;
            case ARRAY_V    : value->astrdecoded = astrvec(value->value.vec); break;
            case OBJECT_V   : value->astrdecoded = astrmap(value->value.map); break;
            case BUILTIN_V  : value->astrdecoded = astrfunc(value->value.bfn); break;
            case USERFUNC_V : value->astrdecoded = astrufunc(value->value.ufn); break;
            case NODE_V     : value->astrdecoded = strdup("NODE"); break;
            default         : die("Invalid value type '%c' (%d)\n", value->type, value->type); break;
        }

        /* Default */
        if(!value->astrdecoded) {
            value->astrdecoded = strdup("null");
        }

        return value->astrdecoded;
    }

    return "null";
}


char* strencoded(VALUE* value) {
    if(value) {
        /* Reset the previous representation */
        if(value->astrencoded && value->type != NULL_V) {
            free(value->astrencoded);
            value->astrencoded = NULL;
        }

        /* Generate new representation */
        switch(value->type) {
            case NULL_V     : value->astrencoded = strdup("null"); break;
            case BOOL_V     : value->astrencoded = value->value.i64 ? strdup("true") : strdup("false"); break;
            case INT64_V    : value->astrencoded = astri64(value->value.i64); break;
            case DOUBLE_V   : value->astrencoded = astrf64(value->value.f64); break;
            case STRING_V   : value->astrencoded = astrencode(value->value.str); break;
            case ARRAY_V    : value->astrencoded = astrvec(value->value.vec); break;
            case OBJECT_V   : value->astrencoded = astrmap(value->value.map); break;
            case BUILTIN_V  : value->astrencoded = astrfunc(value->value.bfn); break;
            case USERFUNC_V : value->astrencoded = astrufunc(value->value.ufn); break;
            case NODE_V     : value->astrencoded = strdup("NODE"); break;
            default         : die("Invalid value type '%c' (%d)\n", value->type, value->type); break;
        }

        /* Default */
        if(!value->astrencoded) {
            value->astrencoded = strdup("null");
        }

        return value->astrencoded;
    }

    return "null";
}


int cmpvalue(VALUE* value1, VALUE* value2) {
    int result = 0;

    if     (value1->type == NULL_V     && value2->type == NULL_V    ) result = 0;
    else if(value1->type == BOOL_V     && value2->type == BOOL_V    ) result = value1->value.i64 < value2->value.i64 ? -1 : value1->value.i64 == value2->value.i64 ? 0 : 1;
    else if(value1->type == INT64_V    && value2->type == INT64_V   ) result = value1->value.i64 < value2->value.i64 ? -1 : value1->value.i64 == value2->value.i64 ? 0 : 1;
    else if(value1->type == INT64_V    && value2->type == DOUBLE_V  ) result = value1->value.i64 < value2->value.f64 ? -1 : value1->value.i64 == value2->value.f64 ? 0 : 1;
    else if(value1->type == DOUBLE_V   && value2->type == INT64_V   ) result = value1->value.f64 < value2->value.i64 ? -1 : value1->value.f64 == value2->value.i64 ? 0 : 1;
    else if(value1->type == DOUBLE_V   && value2->type == DOUBLE_V  ) result = value1->value.f64 < value2->value.f64 ? -1 : value1->value.f64 == value2->value.f64 ? 0 : 1;
    else if(value1->type == STRING_V   && value2->type == STRING_V  ) result = strcmp(value1->value.str, value2->value.str);
    else if(value1->type == ARRAY_V    && value2->type == ARRAY_V   ) result = cmpvec(value1->value.vec, value2->value.vec);
    else if(value1->type == OBJECT_V   && value2->type == OBJECT_V  ) result = cmpmap(value1->value.map, value2->value.map);
    else if(value1->type == BUILTIN_V  && value2->type == BUILTIN_V ) result = value1->value.bfn  < value2->value.bfn  ? -1 : value1->value.bfn  == value2->value.bfn  ? 0 : 1;
    else if(value1->type == USERFUNC_V && value2->type == USERFUNC_V) result = value1->value.ufn  < value2->value.ufn  ? -1 : value1->value.ufn  == value2->value.ufn  ? 0 : 1;
    else if(value1->type == NODE_V     && value2->type == NODE_V    ) result = value1->value.node < value2->value.node ? -1 : value1->value.node == value2->value.node ? 0 : 1;
    else                                                              result = value2->type - value1->type;

    return result;
}


int gettype(VALUE* value) {
    return value ? value->type : NULL_V;
}


int64_t getint64(VALUE* value) {
    return value->value.i64;
}


double getdouble(VALUE* value) {
    return value->value.f64;
}


char* getstring(VALUE* value) {
    return value->value.str;
}


VEC* getarray(VALUE* value) {
    return value->value.vec;
}


MAP* getobject(VALUE* value) {
    return value->value.map;
}


FUNC* getbuiltin(VALUE* value) {
    return value->value.bfn;
}


UFUNC* getuserfunc(VALUE* value) {
    return value->value.ufn;
}
