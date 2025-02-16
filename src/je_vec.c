#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "je_error.h"
#include "je_util.h"
#include "je_val.h"
#include "je_vec.h"


/* ***************************************************************************
* CONSTANTS
*/

#define _INITSIZE 8


/* ***************************************************************************
* MACROS
*/

#define _MAX(a,b) ((a)>(b)?(a):(b))


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_VEC* JE_VecNew() {
    JE_VEC* vec = JE_Calloc(1, sizeof(JE_VEC));

    vec->item = JE_Calloc(_INITSIZE, sizeof(JE_VAL*));
    vec->count = 1;
    vec->length = 0;
    vec->capacity = _INITSIZE;

    return vec;
}

JE_VEC* JE_VecDup(JE_VEC* vec) {
    vec->count++;

    return vec;
}

void JE_VecDelete(JE_VEC* vec) {
    vec->count--;

    if(vec->count == 0) {
        for(size_t i=0; i<vec->length; i++) {
            JE_ValDelete(vec->item[i]);
        }

        if(vec->item) free(vec->item);
        vec->item = NULL;
        vec->capacity = 0;
        vec->length = 0;
        vec->count = 0;
        free(vec);
    }
}

int JE_VecSet(JE_VEC* vec, size_t index, JE_VAL* item) {
    int isok = 1;

    if(index == vec->length) {
        JE_VecPush(vec, item);
    }
    else if(index < vec->length) {
        JE_ValDelete(vec->item[index]);
        vec->item[index] = item;
    }
    else {
        JE_ThrowLater("Insert to invalid index %zd (max %zd)", index, vec->length);
        isok = 0;
    }

    return isok;
}

void JE_VecPop(JE_VEC* vec) {
    if(vec->length) {
        JE_ValDelete(vec->item[vec->length-1]);
        vec->length--;
    }
}

void JE_VecPush(JE_VEC* vec, JE_VAL* item) {
    /* Allocate more memory if needed */
    if(vec->length >= vec->capacity) {
        vec->item = JE_Realloc(vec->item, vec->capacity*2, sizeof(JE_VAL*));
        vec->capacity *= 2;
    }

    /* Insert */
    vec->item[vec->length] = item;
    vec->length++;
}

void JE_VecUnset(JE_VEC* vec, size_t index) {
    if(index < vec->length) {
        JE_ValDelete(vec->item[index]);

        for(size_t i=index; i<vec->length-1; i++) {
            vec->item[i] = vec->item[i+1];
        }

        vec->length--;
    }
}

JE_VAL* JE_VecGet(JE_VEC* vec, size_t index) {
    JE_VAL* item = NULL;

    if(index < vec->length) {
        item = vec->item[index];
    }

    return item;
}

JE_VAL* JE_VecBack(JE_VEC* vec) {
    JE_VAL* item = NULL;

    if(vec->length) item = vec->item[vec->length-1];

    return item;
}

int JE_VecCmp(JE_VEC* vec1, JE_VEC* vec2) {
    size_t len = _MAX(vec1->length, vec2->length);
    int cmp = 0;

    for(size_t i=0; i<len; i++) {
        if     (i<vec1->length && i<vec2->length) cmp = JE_ValCmp(vec1->item[i], vec2->item[i]);
        else if(i<vec1->length)                   cmp = 1;
        else                                      cmp = -1;

        if(cmp != 0) break;
    }

    return cmp;
}

char* JE_VecToAstr(JE_VEC* vec) {
    char* str = JE_Calloc(1, strlen("[  ]")+1);

    /* Opening bracket */
    snprintf(str, 2, "[");

    /* Items */
    for(size_t i=0; i<vec->length; i++) {
        if(i > 0) str = JE_AstrCat(str, ",");
        str = JE_AstrCat(str, " ");
        str = JE_AstrCat(str, JE_ValToQstr(vec->item[i]));
    }

    /* Closing bracket */
    str = JE_AstrCat(str, " ]");

    return str;
}

size_t JE_VecLen(JE_VEC* vec) {
    return vec->length;
}


/* ***************************************************************************
* TEST FUNCTIONS
*/

void _JE_VecTest() {
    JE_VEC* vec = JE_VecNew();

    JE_VecPush(vec, JE_ValNewFromCstr("Hello, world!"));
    JE_VecPush(vec, JE_ValNewFromCstr("Bye, world!"));

    for(int i=0; i<vec->length; i++) {
        JE_VAL* val = vec->item[i];

        printf("%s\n", val->value.s);
    }

    JE_VecDelete(vec);
}
