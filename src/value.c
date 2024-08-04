#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "func.h"
#include "map.h"
#include "util.h"
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

VALUE* newnull() {
    if(!null) {
        null = calloc(1, sizeof(VALUE));

        null->type = NULL_V;
        null->count = 1;
    }

    return null;
}


VALUE* newbool(int64_t i64) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = BOOL_V;
    value->count = 1;
    value->value.i64 = i64;

    return value;
}


VALUE* newint64(int64_t i64) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = INT64_V;
    value->count = 1;
    value->value.i64 = i64;

    return value;
}


VALUE* newdouble(double f64) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = DOUBLE_V;
    value->count = 1;
    value->value.f64 = f64;

    return value;
}


VALUE* newstring(const char* str) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = STRING_V;
    value->count = 1;
    value->value.str = strdup(str);

    return value;
}


VALUE* newarray(VEC* vec) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = ARRAY_V;
    value->count = 1;
    value->value.vec = vec ? vec : newvec();

    return value;
}


VALUE* newobject(MAP* map) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = OBJECT_V;
    value->count = 1;
    value->value.map = map ? map : newmap();

    return value;
}


VALUE* newbuiltin(FUNC* func) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = BUILTIN_V;
    value->count = 1;
    value->value.bfn = func;

    return value;
}


VALUE* newuserfunc(UFUNC* ufunc) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = USERFUNC_V;
    value->count = 1;
    value->value.ufn = ufunc;

    return value;
}


VALUE* newnodevalue(NODE* node) {
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
            default         : fprintf(stderr, "%s: Invalid value type '%c' (%d)\n", __FUNCTION__, value->type, value->type); break;
        }

        if(value->astrvalue) free(value->astrvalue);
        if(value->astrencoded) free(value->astrencoded);
        free(value);
    }
}


char* strvalue(VALUE* value) {
    if(value) {
        /* Reset the previous representation */
        if(value->astrvalue && value->type != NULL_V) {
            free(value->astrvalue);
            value->astrvalue = NULL;
        }

        /* Generate new representation */
        switch(value->type) {
            case NULL_V     : value->astrvalue = strdup("null"); break;
            case BOOL_V     : value->astrvalue = value->value.i64 ? strdup("true") : strdup("false"); break;
            case INT64_V    : value->astrvalue = astri64(value->value.i64); break;
            case DOUBLE_V   : value->astrvalue = astrf64(value->value.f64); break;
            case STRING_V   : value->astrvalue = strdup(value->value.str); break;
            case ARRAY_V    : value->astrvalue = astrvec(value->value.vec); break;
            case OBJECT_V   : value->astrvalue = astrmap(value->value.map); break;
            case BUILTIN_V  : value->astrvalue = astrfunc(value->value.bfn); break;
            case USERFUNC_V : value->astrvalue = astrufunc(value->value.ufn); break;
            case NODE_V     : value->astrvalue = strdup("NODE"); break;
            default         : fprintf(stderr, "%s: Invalid value type '%c' (%d)\n", __FUNCTION__, value->type, value->type); break;
        }

        /* Default */
        if(!value->astrvalue) {
            value->astrvalue = strdup("null");
        }

        return value->astrvalue;
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
            default         : fprintf(stderr, "%s: Invalid value type '%c' (%d)\n", __FUNCTION__, value->type, value->type); break;
        }

        /* Default */
        if(!value->astrencoded) {
            value->astrencoded = strdup("null");
        }

        return value->astrencoded;
    }

    return "null";
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
