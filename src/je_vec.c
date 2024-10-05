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

#define INITSIZE 8


/* ***************************************************************************
* MACROS
*/

#define MAX(a,b) ((a)>(b)?(a):(b))


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_VEC* je_newvec() {
    JE_VEC* vec = calloc(1, sizeof(JE_VEC));

    vec->item = calloc(INITSIZE, sizeof(JE_VAL*));
    vec->count = 1;
    vec->length = 0;
    vec->capacity = INITSIZE;

    return vec;
}


JE_VEC* je_dupvec(JE_VEC* vec) {
    vec->count++;

    return vec;
}


void je_freevec(JE_VEC* vec) {
    vec->count--;

    if(vec->count == 0) {
        for(size_t i=0; i<vec->length; i++) {
            je_freeval(vec->item[i]);
        }

        if(vec->item) free(vec->item);
        vec->item = NULL;
        vec->capacity = 0;
        vec->length = 0;
        vec->count = 0;
        free(vec);
    }
}


int je_vecset(JE_VEC* vec, size_t index, JE_VAL* item) {
    int isok = 1;

    if(index == vec->length) {
        je_vecpush(vec, item);
    }
    else if(index < vec->length) {
        je_freeval(vec->item[index]);
        vec->item[index] = item;
    }
    else {
        je_throwLater("Insert to invalid index %zd (max %zd)", index, vec->length);
        isok = 0;
    }

    return isok;
}


void je_vecpop(JE_VEC* vec) {
    if(vec->length) {
        je_freeval(vec->item[vec->length-1]);
        vec->length--;
    }
}


void je_vecpush(JE_VEC* vec, JE_VAL* item) {
    /* Allocate more memory if needed */
    if(vec->length >= vec->capacity) {
        vec->item = realloc(vec->item, vec->capacity*2 * sizeof(JE_VAL*));
        vec->capacity *= 2;
    }

    /* Insert */
    vec->item[vec->length] = item;
    vec->length++;
}


void je_vecunset(JE_VEC* vec, size_t index) {
    if(index < vec->length) {
        je_freeval(vec->item[index]);

        for(size_t i=index; i<vec->length-1; i++) {
            vec->item[i] = vec->item[i+1];
        }

        vec->length--;
    }
}


JE_VAL* je_vecget(JE_VEC* vec, size_t index) {
    JE_VAL* item = NULL;

    if(index < vec->length) {
        item = vec->item[index];
    }

    return item;
}


JE_VAL* je_vecback(JE_VEC* vec) {
    JE_VAL* item = NULL;

    if(vec->length) item = vec->item[vec->length-1];

    return item;
}


int je_veccmp(JE_VEC* vec1, JE_VEC* vec2) {
    size_t len = MAX(vec1->length, vec2->length);
    int cmp = 0;

    for(size_t i=0; i<len; i++) {
        if     (i<vec1->length && i<vec2->length) cmp = je_valcmp(vec1->item[i], vec2->item[i]);
        else if(i<vec1->length)                   cmp = 1;
        else                                      cmp = -1;

        if(cmp != 0) break;
    }

    return cmp;
}


char* je_vecastr(JE_VEC* vec) {
    char* str = calloc(1, strlen("[  ]")+1);

    /* Opening bracket */
    snprintf(str, 2, "[");

    /* Items */
    for(size_t i=0; i<vec->length; i++) {
        if(i > 0) str = je_astrcat(str, ",");
        str = je_astrcat(str, " ");
        str = je_astrcat(str, je_valqstr(vec->item[i]));
    }

    /* Closing bracket */
    str = je_astrcat(str, " ]");

    return str;
}


size_t je_veclen(JE_VEC* vec) {
    return vec->length;
}


/* ***************************************************************************
* TEST FUNCTIONS
*/

void _je_testvec() {
    JE_VEC* vec = je_newvec();

    je_vecpush(vec, je_strval("Hello, world!"));
    je_vecpush(vec, je_strval("Bye, world!"));

    for(int i=0; i<vec->length; i++) {
        JE_VAL* val = vec->item[i];

        printf("%s\n", val->value.s);
    }

    je_freevec(vec);
}
