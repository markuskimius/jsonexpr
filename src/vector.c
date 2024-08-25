#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "throw.h"
#include "value.h"
#include "vector.h"


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

VEC* newvec() {
    VEC* vec = calloc(1, sizeof(VEC));

    vec->item = calloc(INITSIZE, sizeof(VALUE*));
    vec->count = 1;
    vec->length = 0;
    vec->capacity = INITSIZE;

    return vec;
}


VEC* dupvec(VEC* vec) {
    vec->count++;

    return vec;
}


void freevec(VEC* vec) {
    vec->count--;

    if(vec->count == 0) {
        for(size_t i=0; i<vec->length; i++) {
            freevalue(vec->item[i]);
        }

        if(vec->item) free(vec->item);
        free(vec);
    }
}


char* astrvec(VEC* vec) {
    char* str = calloc(1, strlen("[  ]")+1);

    /* Opening bracket */
    snprintf(str, 2, "[");

    /* Items */
    for(size_t i=0; i<vec->length; i++) {
        if(i > 0) str = astrcat(str, ",");
        str = astrcat(str, " ");
        str = astrcat(str, strencoded(vec->item[i]));
    }

    /* Closing bracket */
    str = astrcat(str, " ]");

    return str;
}


void pushvec(VEC* vec, VALUE* item) {
    /* Allocate more memory if needed */
    if(vec->length >= vec->capacity) {
        vec->item = realloc(vec->item, vec->capacity*2 * sizeof(VALUE*));
        vec->capacity *= 2;
    }

    /* Insert */
    vec->item[vec->length] = item;
    vec->length++;
}


int setvec(VEC* vec, size_t index, VALUE* item) {
    int isok = 1;

    if(index == vec->length) {
        pushvec(vec, item);
    }
    else if(index < vec->length) {
        freevalue(vec->item[index]);
        vec->item[index] = item;
    }
    else {
        raise("Insert to invalid index %zd (max %zd)", index, vec->length);
        isok = 0;
    }

    return isok;
}


void popvec(VEC* vec) {
    if(vec->length) {
        freevalue(vec->item[vec->length-1]);
        vec->length--;
    }
}


VALUE* getvec(VEC* vec, size_t index) {
    VALUE* item = NULL;

    if(index < vec->length) {
        item = vec->item[index];
    }

    return item;
}


VALUE* backvec(VEC* vec) {
    VALUE* item = NULL;

    if(vec->length) item = vec->item[vec->length-1];

    return item;
}


/* ***************************************************************************
* TEST FUNCTIONS
*/

void _testvec() {
    VEC* vec = newvec();

    pushvec(vec, strvalue("Hello, world!"));
    pushvec(vec, strvalue("Bye, world!"));

    for(int i=0; i<vec->length; i++) {
        VALUE* value = vec->item[i];

        printf("%s\n", value->value.s);
    }

    freevec(vec);
}


int cmpvec(VEC* vec1, VEC* vec2) {
    size_t len = MAX(vec1->length, vec2->length);
    int cmp = 0;

    for(size_t i=0; i<len; i++) {
        if     (i<vec1->length && i<vec2->length) cmp = cmpvalue(vec1->item[i], vec2->item[i]);
        else if(i<vec1->length)                   cmp = 1;
        else                                      cmp = -1;

        if(cmp != 0) break;
    }

    return cmp;
}
