#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "value.h"
#include "vector.h"


/* ***************************************************************************
* CONSTANTS
*/

#define INITSIZE 8


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VEC* newvec() {
    VEC* vec = calloc(1, sizeof(VEC));

    vec->item = calloc(INITSIZE, sizeof(VALUE*));
    vec->length = 0;
    vec->capacity = INITSIZE;

    return vec;
}


void freevec(VEC* vec) {
    for(size_t i=0; i<vec->length; i++) {
        freevalue(vec->item[i]);
    }

    if(vec->item) free(vec->item);
    free(vec);
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
    if(vec->length+1 >= vec->capacity) {
        vec->item = realloc(vec->item, vec->capacity*2);
        vec->capacity *= 2;
    }

    /* Insert */
    vec->item[vec->length] = item;
    vec->length++;
}


void setvec(VEC* vec, size_t index, VALUE* item) {
    if(index == vec->length) {
        pushvec(vec, item);
    }
    else if(index < vec->length) {
        freevalue(vec->item[index]);
        vec->item[index] = item;
    }
    else {
        fprintf(stderr, "%s: Insert to invalid index %zd (max %zd)\n", __FUNCTION__, index, vec->length);
    }
}


void popvec(VEC* vec) {
    if(vec->length) {
        freevalue(vec->item[vec->length-1]);
        vec->length--;
    }
}


VALUE* getvec(VEC* vec, size_t index) {
    VALUE* item = NULL;

    if(index < vec->length) item = vec->item[index];

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

    pushvec(vec, newstring("Hello, world!"));
    pushvec(vec, newstring("Bye, world!"));

    for(int i=0; i<vec->length; i++) {
        VALUE* value = vec->item[i];

        printf("%s\n", value->value.str);
    }

    freevec(vec);
}
