#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "func.h"
#include "map.h"
#include "util.h"
#include "val.h"
#include "vec.h"


/* ***************************************************************************
* CONSTANTS
*/

#define NAMEMAX 16


/* ***************************************************************************
* GLOBALS
*/

static char VALNAME[128][NAMEMAX];


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VAL* nullval() {
    VAL* null = calloc(1, sizeof(VAL));

    null->type = NULL_V;

    return null;
}


VAL* boolval(int64_t i64) {
    VAL* val = calloc(1, sizeof(VAL));

    val->type = BOOL_V;
    val->value.i = i64 ? 1 : 0;

    return val;
}


VAL* intval(int64_t i64) {
    VAL* val = calloc(1, sizeof(VAL));

    val->type = INT_V;
    val->value.i = i64;

    return val;
}


VAL* dblval(double f64) {
    VAL* val = calloc(1, sizeof(VAL));

    val->type = FLOAT_V;
    val->value.f = f64;

    return val;
}


VAL* strval(char* str) {
    VAL* val = calloc(1, sizeof(VAL));

    val->type = STRING_V;
    val->value.s = strdup(str);

    return val;
}


VAL* arrval(VEC* vec) {
    VAL* val = calloc(1, sizeof(VAL));

    val->type = ARRAY_V;
    val->value.v = vec ? vec : newvec();

    return val;
}


VAL* objval(MAP* map) {
    VAL* val = calloc(1, sizeof(VAL));

    val->type = OBJECT_V;
    val->value.m = map ? map : newmap();

    return val;
}


VAL* funcval(FUNC* func) {
    VAL* val = calloc(1, sizeof(VAL));

    val->type = FUNCTION_V;
    val->value.fn = func;

    return val;
}


VAL* nodeval(NODE* node) {
    VAL* val = calloc(1, sizeof(VAL));

    val->type = NODE_V;
    val->value.n = node;

    return val;
}


VAL* dupval(VAL* val) {
    switch(val->type) {
        case NULL_V     : return nullval();
        case BOOL_V     : return boolval(val->value.i);
        case INT_V      : return intval(val->value.i);
        case FLOAT_V    : return dblval(val->value.f);
        case STRING_V   : return strval(strdup(val->value.s));
        case ARRAY_V    : return arrval(dupvec(val->value.v));
        case OBJECT_V   : return objval(dupmap(val->value.m));
        case FUNCTION_V : return funcval(dupfunc(val->value.fn));
        case NODE_V     : return nodeval(val->value.n);
        default         : die("Invalid val type '%c' (%d)\n", val->type, val->type); break;
    }

    return val;
}


void swapval(VAL* val1, VAL* val2) {
    VAL* tmp = malloc(sizeof(VAL));

    memcpy(tmp, val1, sizeof(VAL));
    memcpy(val1, val2, sizeof(VAL));
    memcpy(val2, tmp, sizeof(VAL));
}


void freeval(VAL* val) {
    switch(val->type) {
        case NULL_V     : break;
        case BOOL_V     : break;
        case INT_V      : break;
        case FLOAT_V    : break;
        case STRING_V   : free(val->value.s); break;
        case ARRAY_V    : freevec(val->value.v); break;
        case OBJECT_V   : freemap(val->value.m); break;
        case FUNCTION_V : freefunc(val->value.fn); break;
        case NODE_V     : break;
        default         : die("Invalid val type '%c' (%d)\n", val->type, val->type); break;
    }

    if(val->astrdecoded) free(val->astrdecoded);
    if(val->astrencoded) free(val->astrencoded);

    val->astrencoded = NULL;
    val->astrdecoded = NULL;
    val->value.s = NULL;
    free(val);
}


int valtrue(VAL* val) {
    int result = 0;

    if     (val->type == NULL_V    ) result = 0;
    else if(val->type == BOOL_V    ) result = val->value.i ? 1 : 0;
    else if(val->type == INT_V     ) result = val->value.i ? 1 : 0;
    else if(val->type == FLOAT_V   ) result = val->value.f ? 1 : 0;
    else if(val->type == STRING_V  ) result = strlen(val->value.s) ? 1 : 0;
    else if(val->type == ARRAY_V   ) result = val->value.v->length ? 1 : 0;
    else if(val->type == OBJECT_V  ) result = val->value.m->length ? 1 : 0;
    else if(val->type == FUNCTION_V) result = 1;
    else if(val->type == NODE_V    ) result = 1;

    return result;
}


int valcmp(VAL* val1, VAL* val2) {
    int result = 0;

    if     (val1->type == NULL_V     && val2->type == NULL_V    ) result = 0;
    else if(val1->type == BOOL_V     && val2->type == BOOL_V    ) result = val1->value.i < val2->value.i ? -1 : val1->value.i == val2->value.i ? 0 : 1;
    else if(val1->type == INT_V      && val2->type == INT_V     ) result = val1->value.i < val2->value.i ? -1 : val1->value.i == val2->value.i ? 0 : 1;
    else if(val1->type == INT_V      && val2->type == FLOAT_V   ) result = val1->value.i < val2->value.f ? -1 : val1->value.i == val2->value.f ? 0 : 1;
    else if(val1->type == FLOAT_V    && val2->type == INT_V     ) result = val1->value.f < val2->value.i ? -1 : val1->value.f == val2->value.i ? 0 : 1;
    else if(val1->type == FLOAT_V    && val2->type == FLOAT_V   ) result = val1->value.f < val2->value.f ? -1 : val1->value.f == val2->value.f ? 0 : 1;
    else if(val1->type == STRING_V   && val2->type == STRING_V  ) result = strcmp(val1->value.s, val2->value.s);
    else if(val1->type == ARRAY_V    && val2->type == ARRAY_V   ) result = veccmp(val1->value.v, val2->value.v);
    else if(val1->type == OBJECT_V   && val2->type == OBJECT_V  ) result = mapcmp(val1->value.m, val2->value.m);
    else if(val1->type == FUNCTION_V && val2->type == FUNCTION_V) result = val1->value.fn < val2->value.fn ? -1 : val1->value.fn == val2->value.fn ? 0 : 1;
    else if(val1->type == NODE_V     && val2->type == NODE_V    ) result = val1->value.n < val2->value.n ? -1 : val1->value.n == val2->value.n ? 0 : 1;
    else                                                          result = val2->type - val1->type;

    return result;
}


char* valstr(VAL* val) {
    if(val) {
        /* Reset the previous representation */
        if(val->astrdecoded && val->type != NULL_V) {
            free(val->astrdecoded);
            val->astrdecoded = NULL;
        }

        /* Generate new representation */
        switch(val->type) {
            case NULL_V     : val->astrdecoded = strdup("null"); break;
            case BOOL_V     : val->astrdecoded = val->value.i ? strdup("true") : strdup("false"); break;
            case INT_V      : val->astrdecoded = astri64(val->value.i); break;
            case FLOAT_V    : val->astrdecoded = astrf64(val->value.f); break;
            case STRING_V   : val->astrdecoded = strdup(val->value.s); break;
            case ARRAY_V    : val->astrdecoded = vecastr(val->value.v); break;
            case OBJECT_V   : val->astrdecoded = mapastr(val->value.m); break;
            case FUNCTION_V : val->astrdecoded = funcastr(val->value.fn); break;
            case NODE_V     : val->astrdecoded = strdup("NODE"); break;
            default         : die("Invalid val type '%c' (%d)\n", val->type, val->type); break;
        }

        /* Default */
        if(!val->astrdecoded) {
            val->astrdecoded = strdup("null");
        }

        return val->astrdecoded;
    }

    return "null";
}


char* valqstr(VAL* val) {
    if(val) {
        /* Reset the previous representation */
        if(val->astrencoded && val->type != NULL_V) {
            free(val->astrencoded);
            val->astrencoded = NULL;
        }

        /* Generate new representation */
        switch(val->type) {
            case NULL_V     : val->astrencoded = strdup("null"); break;
            case BOOL_V     : val->astrencoded = val->value.i ? strdup("true") : strdup("false"); break;
            case INT_V      : val->astrencoded = astri64(val->value.i); break;
            case FLOAT_V    : val->astrencoded = astrf64(val->value.f); break;
            case STRING_V   : val->astrencoded = astrencode(val->value.s); break;
            case ARRAY_V    : val->astrencoded = vecastr(val->value.v); break;
            case OBJECT_V   : val->astrencoded = mapastr(val->value.m); break;
            case FUNCTION_V : val->astrencoded = funcastr(val->value.fn); break;
            case NODE_V     : val->astrencoded = strdup("NODE"); break;
            default         : die("Invalid val type '%c' (%d)\n", val->type, val->type); break;
        }

        /* Default */
        if(!val->astrencoded) {
            val->astrencoded = strdup("null");
        }

        return val->astrencoded;
    }

    return "null";
}


const char* valtype(VAL* val) {
    int type = val->type;
    char* name = NULL;

    switch(type) {
        case NULL_V      : snprintf(VALNAME[NULL_V],     NAMEMAX, "NULL");      name = VALNAME[NULL_V];     break;
        case BOOL_V      : snprintf(VALNAME[BOOL_V],     NAMEMAX, "BOOL");      name = VALNAME[BOOL_V];     break;
        case INT_V       : snprintf(VALNAME[INT_V],      NAMEMAX, "INT");       name = VALNAME[INT_V];      break;
        case FLOAT_V     : snprintf(VALNAME[FLOAT_V],    NAMEMAX, "FLOAT");     name = VALNAME[FLOAT_V];    break;
        case STRING_V    : snprintf(VALNAME[STRING_V],   NAMEMAX, "STRING");    name = VALNAME[STRING_V];   break;
        case ARRAY_V     : snprintf(VALNAME[ARRAY_V],    NAMEMAX, "ARRAY");     name = VALNAME[ARRAY_V];    break;
        case OBJECT_V    : snprintf(VALNAME[OBJECT_V],   NAMEMAX, "OBJECT");    name = VALNAME[OBJECT_V];   break;
        case FUNCTION_V  : snprintf(VALNAME[FUNCTION_V], NAMEMAX, "FUNCTION");  name = VALNAME[FUNCTION_V]; break;
        case NODE_V      : snprintf(VALNAME[NODE_V],     NAMEMAX, "NODE");      name = VALNAME[NODE_V];     break;
        default:
            if(isprint(type)) snprintf(VALNAME[0], NAMEMAX, "'%c' (%d)", type, type);
            else snprintf(VALNAME[0], NAMEMAX, "? (%d)", type);

            name = VALNAME[0];
    }

    return name;
}


/* ***************************************************************************
* EXPORTED FUNCTIONS
*/

int gettype(VAL* val) {
    return val ? val->type : NULL_V;
}


int64_t getint64(VAL* val) {
    return val->value.i;
}


double getdouble(VAL* val) {
    return val->value.f;
}


char* getstring(VAL* val) {
    return val->value.s;
}


VEC* getarray(VAL* val) {
    return val->value.v;
}


MAP* getobject(VAL* val) {
    return val->value.m;
}


FUNC* getfunc(VAL* val) {
    return val->value.fn;
}
