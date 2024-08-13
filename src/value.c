#include <ctype.h>
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
* CONSTANTS
*/

#define NAMEMAX 16


/* ***************************************************************************
* GLOBALS
*/

VALUE* null = NULL;

static char VALUENAME[128][NAMEMAX];


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
    value->value.i = i64 ? 1 : 0;

    return value;
}


VALUE* intvalue(int64_t i64) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = INT_V;
    value->count = 1;
    value->value.i = i64;

    return value;
}


VALUE* dblvalue(double f64) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = FLOAT_V;
    value->count = 1;
    value->value.f = f64;

    return value;
}


VALUE* strvalue(char* str) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = STRING_V;
    value->count = 1;
    value->value.s = str;

    return value;
}


VALUE* arrvalue(VEC* vec) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = ARRAY_V;
    value->count = 1;
    value->value.v = vec ? vec : newvec();

    return value;
}


VALUE* objvalue(MAP* map) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = OBJECT_V;
    value->count = 1;
    value->value.m = map ? map : newmap();

    return value;
}


VALUE* bfnvalue(FUNC* func) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = BUILTIN_V;
    value->count = 1;
    value->value.b = func;

    return value;
}


VALUE* ufnvalue(UFUNC* ufunc) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = USERFUNC_V;
    value->count = 1;
    value->value.u = ufunc;

    return value;
}


VALUE* nodevalue(NODE* node) {
    VALUE* value = calloc(1, sizeof(VALUE));

    value->type = NODE_V;
    value->count = 1;
    value->value.n = node;

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
            case INT_V      : break;
            case FLOAT_V    : break;
            case STRING_V   : free(value->value.s); break;
            case ARRAY_V    : freevec(value->value.v); break;
            case OBJECT_V   : freemap(value->value.m); break;
            case BUILTIN_V  : freefunc(value->value.b); break;
            case USERFUNC_V : freeufunc(value->value.u); break;
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
            case BOOL_V     : value->astrdecoded = value->value.i ? strdup("true") : strdup("false"); break;
            case INT_V      : value->astrdecoded = astri64(value->value.i); break;
            case FLOAT_V    : value->astrdecoded = astrf64(value->value.f); break;
            case STRING_V   : value->astrdecoded = strdup(value->value.s); break;
            case ARRAY_V    : value->astrdecoded = astrvec(value->value.v); break;
            case OBJECT_V   : value->astrdecoded = astrmap(value->value.m); break;
            case BUILTIN_V  : value->astrdecoded = astrfunc(value->value.b); break;
            case USERFUNC_V : value->astrdecoded = astrufunc(value->value.u); break;
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
            case BOOL_V     : value->astrencoded = value->value.i ? strdup("true") : strdup("false"); break;
            case INT_V      : value->astrencoded = astri64(value->value.i); break;
            case FLOAT_V    : value->astrencoded = astrf64(value->value.f); break;
            case STRING_V   : value->astrencoded = astrencode(value->value.s); break;
            case ARRAY_V    : value->astrencoded = astrvec(value->value.v); break;
            case OBJECT_V   : value->astrencoded = astrmap(value->value.m); break;
            case BUILTIN_V  : value->astrencoded = astrfunc(value->value.b); break;
            case USERFUNC_V : value->astrencoded = astrufunc(value->value.u); break;
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


const char* valuetype(VALUE* value) {
    int type = value->type;
    char* name = NULL;

    switch(type) {
        case NULL_V      : snprintf(VALUENAME[NULL_V],     NAMEMAX, "NULL");      name = VALUENAME[NULL_V];     break;
        case BOOL_V      : snprintf(VALUENAME[BOOL_V],     NAMEMAX, "BOOL");      name = VALUENAME[BOOL_V];     break;
        case INT_V       : snprintf(VALUENAME[INT_V],      NAMEMAX, "INT");       name = VALUENAME[INT_V];      break;
        case FLOAT_V     : snprintf(VALUENAME[FLOAT_V],    NAMEMAX, "FLOAT");     name = VALUENAME[FLOAT_V];    break;
        case STRING_V    : snprintf(VALUENAME[STRING_V],   NAMEMAX, "STRING");    name = VALUENAME[STRING_V];   break;
        case ARRAY_V     : snprintf(VALUENAME[ARRAY_V],    NAMEMAX, "ARRAY");     name = VALUENAME[ARRAY_V];    break;
        case OBJECT_V    : snprintf(VALUENAME[OBJECT_V],   NAMEMAX, "OBJECT");    name = VALUENAME[OBJECT_V];   break;
        case BUILTIN_V   : snprintf(VALUENAME[BUILTIN_V],  NAMEMAX, "FUNCTION");  name = VALUENAME[BUILTIN_V];  break;
        case USERFUNC_V  : snprintf(VALUENAME[USERFUNC_V], NAMEMAX, "UFUNCTION"); name = VALUENAME[USERFUNC_V]; break;
        case NODE_V      : snprintf(VALUENAME[NODE_V],     NAMEMAX, "NODE");      name = VALUENAME[NODE_V];     break;
        default:
            if(isprint(type)) snprintf(VALUENAME[0], NAMEMAX, "'%c' (%d)", type, type);
            else snprintf(VALUENAME[0], NAMEMAX, "? (%d)", type);

            name = VALUENAME[0];
    }

    return name;
}


int istrue(VALUE* value) {
    int result = 0;

    if     (value->type == NULL_V    ) result = 0;
    else if(value->type == BOOL_V    ) result = value->value.i ? 1 : 0;
    else if(value->type == INT_V     ) result = value->value.i ? 1 : 0;
    else if(value->type == FLOAT_V   ) result = value->value.f ? 1 : 0;
    else if(value->type == STRING_V  ) result = strlen(value->value.s) ? 1 : 0;
    else if(value->type == ARRAY_V   ) result = value->value.v->length ? 1 : 0;
    else if(value->type == OBJECT_V  ) result = value->value.m->length ? 1 : 0;
    else if(value->type == BUILTIN_V ) result = 1;
    else if(value->type == USERFUNC_V) result = 1;
    else if(value->type == NODE_V    ) result = 1;

    return result;
}


int cmpvalue(VALUE* value1, VALUE* value2) {
    int result = 0;

    if     (value1->type == NULL_V     && value2->type == NULL_V    ) result = 0;
    else if(value1->type == BOOL_V     && value2->type == BOOL_V    ) result = value1->value.i < value2->value.i ? -1 : value1->value.i == value2->value.i ? 0 : 1;
    else if(value1->type == INT_V      && value2->type == INT_V     ) result = value1->value.i < value2->value.i ? -1 : value1->value.i == value2->value.i ? 0 : 1;
    else if(value1->type == INT_V      && value2->type == FLOAT_V   ) result = value1->value.i < value2->value.f ? -1 : value1->value.i == value2->value.f ? 0 : 1;
    else if(value1->type == FLOAT_V    && value2->type == INT_V     ) result = value1->value.f < value2->value.i ? -1 : value1->value.f == value2->value.i ? 0 : 1;
    else if(value1->type == FLOAT_V    && value2->type == FLOAT_V   ) result = value1->value.f < value2->value.f ? -1 : value1->value.f == value2->value.f ? 0 : 1;
    else if(value1->type == STRING_V   && value2->type == STRING_V  ) result = strcmp(value1->value.s, value2->value.s);
    else if(value1->type == ARRAY_V    && value2->type == ARRAY_V   ) result = cmpvec(value1->value.v, value2->value.v);
    else if(value1->type == OBJECT_V   && value2->type == OBJECT_V  ) result = cmpmap(value1->value.m, value2->value.m);
    else if(value1->type == BUILTIN_V  && value2->type == BUILTIN_V ) result = value1->value.b < value2->value.b ? -1 : value1->value.b == value2->value.b ? 0 : 1;
    else if(value1->type == USERFUNC_V && value2->type == USERFUNC_V) result = value1->value.u < value2->value.u ? -1 : value1->value.u == value2->value.u ? 0 : 1;
    else if(value1->type == NODE_V     && value2->type == NODE_V    ) result = value1->value.n < value2->value.n ? -1 : value1->value.n == value2->value.n ? 0 : 1;
    else                                                              result = value2->type - value1->type;

    return result;
}


int gettype(VALUE* value) {
    return value ? value->type : NULL_V;
}


int64_t getint64(VALUE* value) {
    return value->value.i;
}


double getdouble(VALUE* value) {
    return value->value.f;
}


char* getstring(VALUE* value) {
    return value->value.s;
}


VEC* getarray(VALUE* value) {
    return value->value.v;
}


MAP* getobject(VALUE* value) {
    return value->value.m;
}


FUNC* getbuiltin(VALUE* value) {
    return value->value.b;
}


UFUNC* getuserfunc(VALUE* value) {
    return value->value.u;
}
