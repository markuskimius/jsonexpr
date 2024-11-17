#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "je_error.h"
#include "je_func.h"
#include "je_map.h"
#include "je_node.h"
#include "je_util.h"
#include "je_val.h"
#include "je_vec.h"


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

JE_VAL* je_nullval() {
    JE_VAL* null = calloc(1, sizeof(JE_VAL));

    null->type = JE_NULL_V;

    return null;
}


JE_VAL* je_boolval(int64_t i64) {
    JE_VAL* val = calloc(1, sizeof(JE_VAL));

    val->type = JE_BOOL_V;
    val->value.i = i64 ? 1 : 0;

    return val;
}


JE_VAL* je_intval(int64_t i64) {
    JE_VAL* val = calloc(1, sizeof(JE_VAL));

    val->type = JE_INT_V;
    val->value.i = i64;

    return val;
}


JE_VAL* je_dblval(double f64) {
    JE_VAL* val = calloc(1, sizeof(JE_VAL));

    val->type = JE_FLOAT_V;
    val->value.f = f64;

    return val;
}


JE_VAL* je_strval(char* str) {
    JE_VAL* val = calloc(1, sizeof(JE_VAL));

    val->type = JE_STRING_V;
    val->value.s = strdup(str);

    return val;
}


JE_VAL* je_arrval(JE_VEC* vec) {
    JE_VAL* val = calloc(1, sizeof(JE_VAL));

    val->type = JE_ARRAY_V;
    val->value.v = vec ? vec : je_newvec();

    return val;
}


JE_VAL* je_objval(JE_MAP* map) {
    JE_VAL* val = calloc(1, sizeof(JE_VAL));

    val->type = JE_OBJECT_V;
    val->value.m = map ? map : je_newmap();

    return val;
}


JE_VAL* je_funcval(JE_FUNC* func) {
    JE_VAL* val = calloc(1, sizeof(JE_VAL));

    val->type = JE_FUNCTION_V;
    val->value.fn = func;

    return val;
}


JE_VAL* je_nodeval(JE_NODE* node) {
    JE_VAL* val = calloc(1, sizeof(JE_VAL));

    val->type = JE_NODE_V;
    val->value.n = node;

    return val;
}


JE_VAL* je_dupval(JE_VAL* val) {
    switch(val->type) {
        case JE_NULL_V     : return je_nullval();
        case JE_BOOL_V     : return je_boolval(val->value.i);
        case JE_INT_V      : return je_intval(val->value.i);
        case JE_FLOAT_V    : return je_dblval(val->value.f);
        case JE_STRING_V   : return je_strval(strdup(val->value.s));
        case JE_ARRAY_V    : return je_arrval(je_dupvec(val->value.v));
        case JE_OBJECT_V   : return je_objval(je_dupmap(val->value.m));
        case JE_FUNCTION_V : return je_funcval(je_dupfunc(val->value.fn));
        case JE_NODE_V     : return je_nodeval(val->value.n);
        default            : je_die("Invalid val type '%c' (%d)\n", val->type, val->type); break;
    }

    return val;
}


void je_swapval(JE_VAL* val1, JE_VAL* val2) {
    JE_VAL tmp;

    memcpy(&tmp, val1, sizeof(JE_VAL));
    memcpy(val1, val2, sizeof(JE_VAL));
    memcpy(val2, &tmp, sizeof(JE_VAL));
}


void je_freeval(JE_VAL* val) {
    switch(val->type) {
        case JE_NULL_V     : break;
        case JE_BOOL_V     : break;
        case JE_INT_V      : break;
        case JE_FLOAT_V    : break;
        case JE_STRING_V   : free(val->value.s); break;
        case JE_ARRAY_V    : je_freevec(val->value.v); break;
        case JE_OBJECT_V   : je_freemap(val->value.m); break;
        case JE_FUNCTION_V : je_freefunc(val->value.fn); break;
        case JE_NODE_V     : break;
        default            : je_die("Invalid val type '%c' (%d)\n", val->type, val->type); break;
    }

    if(val->astrdecoded) free(val->astrdecoded);
    if(val->astrencoded) free(val->astrencoded);

    val->astrencoded = NULL;
    val->astrdecoded = NULL;
    val->value.s = NULL;
    free(val);
}


int je_valtrue(JE_VAL* val) {
    int result = 0;

    if     (val->type == JE_NULL_V    ) result = 0;
    else if(val->type == JE_BOOL_V    ) result = val->value.i ? 1 : 0;
    else if(val->type == JE_INT_V     ) result = val->value.i ? 1 : 0;
    else if(val->type == JE_FLOAT_V   ) result = val->value.f ? 1 : 0;
    else if(val->type == JE_STRING_V  ) result = strlen(val->value.s) ? 1 : 0;
    else if(val->type == JE_ARRAY_V   ) result = val->value.v->length ? 1 : 0;
    else if(val->type == JE_OBJECT_V  ) result = val->value.m->length ? 1 : 0;
    else if(val->type == JE_FUNCTION_V) result = 1;
    else if(val->type == JE_NODE_V    ) result = 1;

    return result;
}


int je_valcmp(JE_VAL* val1, JE_VAL* val2) {
    int result = 0;

    if     (val1->type == JE_NULL_V     && val2->type == JE_NULL_V    ) result = 0;
    else if(val1->type == JE_BOOL_V     && val2->type == JE_BOOL_V    ) result = val1->value.i < val2->value.i ? -1 : val1->value.i == val2->value.i ? 0 : 1;
    else if(val1->type == JE_INT_V      && val2->type == JE_INT_V     ) result = val1->value.i < val2->value.i ? -1 : val1->value.i == val2->value.i ? 0 : 1;
    else if(val1->type == JE_INT_V      && val2->type == JE_FLOAT_V   ) result = val1->value.i < val2->value.f ? -1 : val1->value.i == val2->value.f ? 0 : 1;
    else if(val1->type == JE_FLOAT_V    && val2->type == JE_INT_V     ) result = val1->value.f < val2->value.i ? -1 : val1->value.f == val2->value.i ? 0 : 1;
    else if(val1->type == JE_FLOAT_V    && val2->type == JE_FLOAT_V   ) result = val1->value.f < val2->value.f ? -1 : val1->value.f == val2->value.f ? 0 : 1;
    else if(val1->type == JE_STRING_V   && val2->type == JE_STRING_V  ) result = strcmp(val1->value.s, val2->value.s);
    else if(val1->type == JE_ARRAY_V    && val2->type == JE_ARRAY_V   ) result = je_veccmp(val1->value.v, val2->value.v);
    else if(val1->type == JE_OBJECT_V   && val2->type == JE_OBJECT_V  ) result = je_mapcmp(val1->value.m, val2->value.m);
    else if(val1->type == JE_FUNCTION_V && val2->type == JE_FUNCTION_V) result = val1->value.fn < val2->value.fn ? -1 : val1->value.fn == val2->value.fn ? 0 : 1;
    else if(val1->type == JE_NODE_V     && val2->type == JE_NODE_V    ) result = val1->value.n < val2->value.n ? -1 : val1->value.n == val2->value.n ? 0 : 1;
    else                                                                result = val2->type - val1->type;

    return result;
}


char* je_valstr(JE_VAL* val) {
    if(val) {
        /* Reset the previous representation */
        if(val->astrdecoded && val->type != JE_NULL_V) {
            free(val->astrdecoded);
            val->astrdecoded = NULL;
        }

        /* Generate new representation */
        switch(val->type) {
            case JE_NULL_V     : val->astrdecoded = strdup("null"); break;
            case JE_BOOL_V     : val->astrdecoded = val->value.i ? strdup("true") : strdup("false"); break;
            case JE_INT_V      : val->astrdecoded = je_astri64(val->value.i); break;
            case JE_FLOAT_V    : val->astrdecoded = je_astrf64(val->value.f); break;
            case JE_STRING_V   : val->astrdecoded = strdup(val->value.s); break;
            case JE_ARRAY_V    : val->astrdecoded = je_vecastr(val->value.v); break;
            case JE_OBJECT_V   : val->astrdecoded = je_mapastr(val->value.m); break;
            case JE_FUNCTION_V : val->astrdecoded = je_funcastr(val->value.fn); break;
            case JE_NODE_V     : val->astrdecoded = je_nodeastr(val->value.n); break;
            default            : je_die("Invalid val type '%c' (%d)\n", val->type, val->type); break;
        }

        /* Default */
        if(!val->astrdecoded) {
            val->astrdecoded = strdup("null");
        }

        return val->astrdecoded;
    }

    return "null";
}


char* je_valqstr(JE_VAL* val) {
    if(val) {
        /* Reset the previous representation */
        if(val->astrencoded && val->type != JE_NULL_V) {
            free(val->astrencoded);
            val->astrencoded = NULL;
        }

        /* Generate new representation */
        switch(val->type) {
            case JE_NULL_V     : val->astrencoded = strdup("null"); break;
            case JE_BOOL_V     : val->astrencoded = val->value.i ? strdup("true") : strdup("false"); break;
            case JE_INT_V      : val->astrencoded = je_astri64(val->value.i); break;
            case JE_FLOAT_V    : val->astrencoded = je_astrf64(val->value.f); break;
            case JE_STRING_V   : val->astrencoded = je_astrencode(val->value.s); break;
            case JE_ARRAY_V    : val->astrencoded = je_vecastr(val->value.v); break;
            case JE_OBJECT_V   : val->astrencoded = je_mapastr(val->value.m); break;
            case JE_FUNCTION_V : val->astrencoded = je_astrencode(je_valstr(val)); break;
            case JE_NODE_V     : val->astrencoded = je_astrencode(je_valstr(val)); break;
            default            : je_die("Invalid val type '%c' (%d)\n", val->type, val->type); break;
        }

        /* Default */
        if(!val->astrencoded) {
            val->astrencoded = strdup("null");
        }

        return val->astrencoded;
    }

    return "null";
}


const char* je_valtype(JE_VAL* val) {
    int type = val->type;
    char* name = NULL;

    switch(type) {
        case JE_NULL_V      : snprintf(VALNAME[JE_NULL_V],     NAMEMAX, "NULL");      name = VALNAME[JE_NULL_V];     break;
        case JE_BOOL_V      : snprintf(VALNAME[JE_BOOL_V],     NAMEMAX, "BOOL");      name = VALNAME[JE_BOOL_V];     break;
        case JE_INT_V       : snprintf(VALNAME[JE_INT_V],      NAMEMAX, "INT");       name = VALNAME[JE_INT_V];      break;
        case JE_FLOAT_V     : snprintf(VALNAME[JE_FLOAT_V],    NAMEMAX, "FLOAT");     name = VALNAME[JE_FLOAT_V];    break;
        case JE_STRING_V    : snprintf(VALNAME[JE_STRING_V],   NAMEMAX, "STRING");    name = VALNAME[JE_STRING_V];   break;
        case JE_ARRAY_V     : snprintf(VALNAME[JE_ARRAY_V],    NAMEMAX, "ARRAY");     name = VALNAME[JE_ARRAY_V];    break;
        case JE_OBJECT_V    : snprintf(VALNAME[JE_OBJECT_V],   NAMEMAX, "OBJECT");    name = VALNAME[JE_OBJECT_V];   break;
        case JE_FUNCTION_V  : snprintf(VALNAME[JE_FUNCTION_V], NAMEMAX, "FUNCTION");  name = VALNAME[JE_FUNCTION_V]; break;
        case JE_NODE_V      : snprintf(VALNAME[JE_NODE_V],     NAMEMAX, "NODE");      name = VALNAME[JE_NODE_V];     break;
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

int je_gettype(JE_VAL* val) {
    return val ? val->type : JE_NULL_V;
}


int64_t je_getint64(JE_VAL* val) {
    return val->value.i;
}


double je_getdouble(JE_VAL* val) {
    return val->value.f;
}


char* je_getstring(JE_VAL* val) {
    return val->value.s;
}


JE_VEC* je_getarray(JE_VAL* val) {
    return val->value.v;
}


JE_MAP* je_getobject(JE_VAL* val) {
    return val->value.m;
}


JE_FUNC* je_getfunc(JE_VAL* val) {
    return val->value.fn;
}
