#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "nvector.h"


/* ***************************************************************************
* CONSTANTS
*/

#define INITSIZE 8


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

NVEC* newnvec() {
    NVEC* vec = calloc(1, sizeof(NVEC));

    vec->item = calloc(INITSIZE, sizeof(NODE*));
    vec->length = 0;
    vec->capacity = INITSIZE;

    return vec;
}


void freenvec(NVEC* vec) {
    if(vec->item) free(vec->item);
    free(vec);
}


void pushnvec(NVEC* vec, NODE* item) {
    /* Allocate more memory if needed */
    if(vec->length+1 >= vec->capacity) {
        vec->item = realloc(vec->item, vec->capacity*2);
        vec->capacity *= 2;
    }

    /* Insert */
    vec->item[vec->length] = item;
    vec->length++;
}


void setnvec(NVEC* vec, size_t index, NODE* item) {
    if(index == vec->length) {
        pushnvec(vec, item);
    }
    else if(index < vec->length) {
        vec->item[index] = item;
    }
    else {
        fprintf(stderr, "%s: Insert to invalid index %zd (max %zd)\n", __FUNCTION__, index, vec->length);
    }
}


void popnvec(NVEC* vec) {
    if(vec->length) {
        vec->length--;
    }
}


NODE* getnvec(NVEC* vec, size_t index) {
    NODE* item = NULL;

    if(index < vec->length) item = vec->item[index];

    return item;
}


NODE* backnvec(NVEC* vec) {
    NODE* item = NULL;

    if(vec->length) item = vec->item[vec->length-1];

    return item;
}
