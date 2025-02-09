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

#define _NAMEMAX 16


/* ***************************************************************************
* GLOBALS
*/

static char _VALNAME[128][_NAMEMAX];


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_VAL* JE_ValNewFromNull() {
    JE_VAL* null = JE_Calloc(1, sizeof(JE_VAL));

    null->type = JE_NULL_V;

    return null;
}

JE_VAL* JE_ValNewFromBool(int64_t i64) {
    JE_VAL* val = JE_Calloc(1, sizeof(JE_VAL));

    val->type = JE_BOOL_V;
    val->value.i = i64 ? 1 : 0;

    return val;
}

JE_VAL* JE_ValNewFromInt(int64_t i64) {
    JE_VAL* val = JE_Calloc(1, sizeof(JE_VAL));

    val->type = JE_INT_V;
    val->value.i = i64;

    return val;
}

JE_VAL* JE_ValNewFromFloat(double f64) {
    JE_VAL* val = JE_Calloc(1, sizeof(JE_VAL));

    val->type = JE_FLOAT_V;
    val->value.f = f64;

    return val;
}

JE_VAL* JE_ValNewFromCstr(const char* str) {
    JE_VAL* val = JE_Calloc(1, sizeof(JE_VAL));

    val->type = JE_STRING_V;
    val->value.s = strdup(str);

    return val;
}

JE_VAL* JE_ValNewFromVec(JE_VEC* vec) {
    JE_VAL* val = JE_Calloc(1, sizeof(JE_VAL));

    val->type = JE_ARRAY_V;
    val->value.v = vec ? vec : JE_VecNew();

    return val;
}

JE_VAL* JE_ValNewFromMap(JE_MAP* map) {
    JE_VAL* val = JE_Calloc(1, sizeof(JE_VAL));

    val->type = JE_OBJECT_V;
    val->value.m = map ? map : JE_MapNew();

    return val;
}

JE_VAL* JE_ValNewFromFunc(JE_FUNC* func) {
    JE_VAL* val = JE_Calloc(1, sizeof(JE_VAL));

    val->type = JE_FUNCTION_V;
    val->value.fn = func;

    return val;
}

JE_VAL* JE_ValNewFromNode(JE_NODE* node) {
    JE_VAL* val = JE_Calloc(1, sizeof(JE_VAL));

    val->type = JE_NODE_V;
    val->value.n = node;

    return val;
}

JE_VAL* JE_ValDup(JE_VAL* val) {
    switch(val->type) {
        case JE_NULL_V     : return JE_ValNewFromNull();
        case JE_BOOL_V     : return JE_ValNewFromBool(val->value.i);
        case JE_INT_V      : return JE_ValNewFromInt(val->value.i);
        case JE_FLOAT_V    : return JE_ValNewFromFloat(val->value.f);
        case JE_STRING_V   : return JE_ValNewFromCstr(strdup(val->value.s));
        case JE_ARRAY_V    : return JE_ValNewFromVec(JE_VecDup(val->value.v));
        case JE_OBJECT_V   : return JE_ValNewFromMap(JE_MapDup(val->value.m));
        case JE_FUNCTION_V : return JE_ValNewFromFunc(JE_FuncDup(val->value.fn));
        case JE_NODE_V     : return JE_ValNewFromNode(val->value.n);
        default            : je_die("Invalid val type '%c' (%d)\n", val->type, val->type); break;
    }

    return val;
}

void JE_ValSwap(JE_VAL* val1, JE_VAL* val2) {
    JE_VAL tmp;

    memcpy(&tmp, val1, sizeof(JE_VAL));
    memcpy(val1, val2, sizeof(JE_VAL));
    memcpy(val2, &tmp, sizeof(JE_VAL));
}

void JE_ValDelete(JE_VAL* val) {
    switch(val->type) {
        case JE_NULL_V     : break;
        case JE_BOOL_V     : break;
        case JE_INT_V      : break;
        case JE_FLOAT_V    : break;
        case JE_STRING_V   : JE_Free(val->value.s); break;
        case JE_ARRAY_V    : JE_VecDelete(val->value.v); break;
        case JE_OBJECT_V   : JE_MapDelete(val->value.m); break;
        case JE_FUNCTION_V : JE_FuncDelete(val->value.fn); break;
        case JE_NODE_V     : break;
        default            : je_die("Invalid val type '%c' (%d)\n", val->type, val->type); break;
    }

    if(val->astrdecoded) JE_Free(val->astrdecoded);
    if(val->astrencoded) JE_Free(val->astrencoded);

    val->astrencoded = NULL;
    val->astrdecoded = NULL;
    val->value.s = NULL;
    JE_Free(val);
}

int JE_ValIsTrue(JE_VAL* val) {
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

int JE_ValCmp(JE_VAL* val1, JE_VAL* val2) {
    int result = 0;

    if     (val1->type == JE_NULL_V     && val2->type == JE_NULL_V    ) result = 0;
    else if(val1->type == JE_BOOL_V     && val2->type == JE_BOOL_V    ) result = val1->value.i < val2->value.i ? -1 : val1->value.i == val2->value.i ? 0 : 1;
    else if(val1->type == JE_INT_V      && val2->type == JE_INT_V     ) result = val1->value.i < val2->value.i ? -1 : val1->value.i == val2->value.i ? 0 : 1;
    else if(val1->type == JE_INT_V      && val2->type == JE_FLOAT_V   ) result = val1->value.i < val2->value.f ? -1 : val1->value.i == val2->value.f ? 0 : 1;
    else if(val1->type == JE_FLOAT_V    && val2->type == JE_INT_V     ) result = val1->value.f < val2->value.i ? -1 : val1->value.f == val2->value.i ? 0 : 1;
    else if(val1->type == JE_FLOAT_V    && val2->type == JE_FLOAT_V   ) result = val1->value.f < val2->value.f ? -1 : val1->value.f == val2->value.f ? 0 : 1;
    else if(val1->type == JE_STRING_V   && val2->type == JE_STRING_V  ) result = strcmp(val1->value.s, val2->value.s);
    else if(val1->type == JE_ARRAY_V    && val2->type == JE_ARRAY_V   ) result = JE_VecCmp(val1->value.v, val2->value.v);
    else if(val1->type == JE_OBJECT_V   && val2->type == JE_OBJECT_V  ) result = JE_MapCmp(val1->value.m, val2->value.m);
    else if(val1->type == JE_FUNCTION_V && val2->type == JE_FUNCTION_V) result = val1->value.fn < val2->value.fn ? -1 : val1->value.fn == val2->value.fn ? 0 : 1;
    else if(val1->type == JE_NODE_V     && val2->type == JE_NODE_V    ) result = val1->value.n < val2->value.n ? -1 : val1->value.n == val2->value.n ? 0 : 1;
    else                                                                result = val2->type - val1->type;

    return result;
}

const char* JE_ValToCstr(JE_VAL* val) {
    if(val) {
        /* Reset the previous representation */
        if(val->astrdecoded && val->type != JE_NULL_V) {
            JE_Free(val->astrdecoded);
            val->astrdecoded = NULL;
        }

        /* Generate new representation */
        switch(val->type) {
            case JE_NULL_V     : val->astrdecoded = strdup("null"); break;
            case JE_BOOL_V     : val->astrdecoded = val->value.i ? strdup("true") : strdup("false"); break;
            case JE_INT_V      : val->astrdecoded = JE_IntToAstr(val->value.i); break;
            case JE_FLOAT_V    : val->astrdecoded = JE_FloatToAstr(val->value.f); break;
            case JE_STRING_V   : val->astrdecoded = strdup(val->value.s); break;
            case JE_ARRAY_V    : val->astrdecoded = JE_VecToAstr(val->value.v); break;
            case JE_OBJECT_V   : val->astrdecoded = JE_MapToAstr(val->value.m); break;
            case JE_FUNCTION_V : val->astrdecoded = JE_FuncToAstr(val->value.fn); break;
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

const char* JE_ValToQstr(JE_VAL* val) {
    if(val) {
        /* Reset the previous representation */
        if(val->astrencoded && val->type != JE_NULL_V) {
            JE_Free(val->astrencoded);
            val->astrencoded = NULL;
        }

        /* Generate new representation */
        switch(val->type) {
            case JE_NULL_V     : val->astrencoded = strdup("null"); break;
            case JE_BOOL_V     : val->astrencoded = val->value.i ? strdup("true") : strdup("false"); break;
            case JE_INT_V      : val->astrencoded = JE_IntToAstr(val->value.i); break;
            case JE_FLOAT_V    : val->astrencoded = JE_FloatToAstr(val->value.f); break;
            case JE_STRING_V   : val->astrencoded = JE_CstrToQstr(val->value.s); break;
            case JE_ARRAY_V    : val->astrencoded = JE_VecToAstr(val->value.v); break;
            case JE_OBJECT_V   : val->astrencoded = JE_MapToAstr(val->value.m); break;
            case JE_FUNCTION_V : val->astrencoded = JE_CstrToQstr(JE_ValToCstr(val)); break;
            case JE_NODE_V     : val->astrencoded = JE_CstrToQstr(JE_ValToCstr(val)); break;
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

const char* JE_ValGetType(JE_VAL* val) {
    int type = val->type;
    char* name = NULL;

    switch(type) {
        case JE_NULL_V      : snprintf(_VALNAME[JE_NULL_V],     _NAMEMAX, "NULL");     name = _VALNAME[JE_NULL_V];     break;
        case JE_BOOL_V      : snprintf(_VALNAME[JE_BOOL_V],     _NAMEMAX, "BOOL");     name = _VALNAME[JE_BOOL_V];     break;
        case JE_INT_V       : snprintf(_VALNAME[JE_INT_V],      _NAMEMAX, "INT");      name = _VALNAME[JE_INT_V];      break;
        case JE_FLOAT_V     : snprintf(_VALNAME[JE_FLOAT_V],    _NAMEMAX, "FLOAT");    name = _VALNAME[JE_FLOAT_V];    break;
        case JE_STRING_V    : snprintf(_VALNAME[JE_STRING_V],   _NAMEMAX, "STRING");   name = _VALNAME[JE_STRING_V];   break;
        case JE_ARRAY_V     : snprintf(_VALNAME[JE_ARRAY_V],    _NAMEMAX, "ARRAY");    name = _VALNAME[JE_ARRAY_V];    break;
        case JE_OBJECT_V    : snprintf(_VALNAME[JE_OBJECT_V],   _NAMEMAX, "OBJECT");   name = _VALNAME[JE_OBJECT_V];   break;
        case JE_FUNCTION_V  : snprintf(_VALNAME[JE_FUNCTION_V], _NAMEMAX, "FUNCTION"); name = _VALNAME[JE_FUNCTION_V]; break;
        case JE_NODE_V      : snprintf(_VALNAME[JE_NODE_V],     _NAMEMAX, "NODE");     name = _VALNAME[JE_NODE_V];     break;
        default:
            if(isprint(type)) snprintf(_VALNAME[0], _NAMEMAX, "'%c' (%d)", type, type);
            else snprintf(_VALNAME[0], _NAMEMAX, "? (%d)", type);

            name = _VALNAME[0];
    }

    return name;
}


/* ***************************************************************************
* EXPORTED FUNCTIONS
*/

int64_t JE_ValGetBool(JE_VAL* val) {
    return val->value.i;
}


int64_t JE_ValGetInt(JE_VAL* val) {
    return val->value.i;
}


double JE_ValGetFloat(JE_VAL* val) {
    return val->value.f;
}


char* JE_ValGetString(JE_VAL* val) {
    return val->value.s;
}


JE_VEC* JE_ValGetVec(JE_VAL* val) {
    return val->value.v;
}


JE_MAP* JE_ValGetMap(JE_VAL* val) {
    return val->value.m;
}


JE_FUNC* JE_ValGetFunc(JE_VAL* val) {
    return val->value.fn;
}
