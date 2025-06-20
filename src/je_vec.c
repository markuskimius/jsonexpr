#include <stdlib.h>
#include <string.h>
#include "je_type.h"
#include "je_util.h"
#include "je_value.h"
#include "je_vec.h"


/* ***************************************************************************
* CONSTANTS
*/

#define _MINSIZE 32


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static void _resize(JE_VEC* vec, size_t nmemb) {
    vec->data = JE_Realloc(vec->data, sizeof(JE_VALUE) * nmemb);
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

/**
* Create a new JE_VEC, optionally specifying the initial capacity.  0 can be
* passed instead of the capacity to default to a reasonable initial capacity.
*/
JE_VEC JE_VecCreate(size_t capacity) {
    JE_VEC vec;

    vec.data = JE_Malloc(sizeof(JE_VALUE) * (capacity ? capacity : _MINSIZE));
    vec.length = 0;

    return vec;
}

/**
* Return a clone of vec.
*/
JE_VEC JE_VecClone(const JE_VEC vec) {
    JE_VEC dup;

    dup.data = vec.length<_MINSIZE ? JE_Malloc(sizeof(JE_VALUE)*_MINSIZE) : JE_Malloc(sizeof(JE_VALUE)*vec.length);
    dup.length = vec.length;

    for(size_t i=0; i<vec.length; i++) {
        dup.data[i] = JE_VCALL(&vec.data[i],clone);
    }

    return dup;
}

/**
* Concatenate two JE_VEC's, return a new JE_VEC.
*/
JE_VEC JE_VecPlusVec(const JE_VEC x, const JE_VEC y) {
    size_t length = x.length + y.length;
    JE_VEC vec;

    vec.data = length<_MINSIZE ? JE_Malloc(sizeof(JE_VALUE)*_MINSIZE) : JE_Malloc(sizeof(JE_VALUE)*(length));
    vec.length = length;

    for(size_t i=0; i<x.length; i++) {
        vec.data[i] = JE_VCALL(&x.data[i],clone);
    }

    for(size_t i=0; i<y.length; i++) {
        vec.data[x.length + i] = JE_VCALL(&y.data[i],clone);
    }

    return vec;
}

/**
* Compare x to y.  Return a value less than, equal to, or greater than 0 if
* x is less than, equal to, or greater than y, respectively.
*/
int64_t JE_VecCompare(const JE_VEC x, const JE_VEC y) {
    size_t length = x.length<y.length ? x.length : y.length;

    for(size_t i=0; i<length; i++) {
        int64_t cmp = JE_VCALL(&x.data[i],compare,y.data[i]);

        if(cmp) return cmp;
    }

    return x.length - y.length;
}

/**
* Concatenate y to x.
*/
void JE_VecPlusAssnVec(JE_VEC* x, const JE_VEC y) {
    size_t length = x->length + y.length;

    _resize(x, length);
    for(size_t i=0; i<y.length; i++) x->data[x->length + i] = JE_VCALL(&y.data[i],clone);
    x->length = length;
}

/**
* Destroy vec.
*/
void JE_VecDestroy(JE_VEC* vec) {
    for(size_t i=0; i<vec->length; i++) {
        JE_VCALL(&vec->data[i],destroy);
    }

    JE_Free(vec->data);
    memset(vec, 0, sizeof(JE_VEC));
}

/**
* Set the value at index i to val.
*
* Once pushed into vec, val may no longer be operated on by the caller.  If it
* needs to be modified after the push, modify the returned pointer instead.
*/
JE_VALUE* JE_VecSet(JE_VEC* vec, size_t i, JE_VALUE val) {
    /* Add more room if needed */
    if(vec->length <= i) {
        JE_VALUE null = JE_ValueNul();

        /* Allocate memory */
        vec->data = JE_Realloc(vec->data, sizeof(JE_VALUE) * (i+1));

        /* Fill it with NUL */
        while(vec->length <= i) {
            vec->data[vec->length++] = null;
        }
    }

    vec->data[i] = val;

    return &vec->data[i];
}

/**
* Push val to the end of vec.
*
* Once pushed into vec, val may no longer be operated on by the caller.  If it
* needs to be modified after the push, modify the returned pointer instead.
*/
JE_VALUE* JE_VecPush(JE_VEC* vec, JE_VALUE val) {
    vec->data = JE_Realloc(vec->data, sizeof(JE_VALUE) * (vec->length+1));
    vec->data[vec->length++] = val;

    return &vec->data[vec->length-1];
}

/**
* Pop the last value in vec.
*
* If vec is empty, return a JE_NUL value instead.
*/
JE_VALUE JE_VecPop(JE_VEC* vec) {
    return vec->length ? vec->data[--vec->length] : JE_ValueNul();
}

/**
* Remove a value at the index in vec.
*/
void JE_VecRemove(JE_VEC* vec, size_t i) {
    if(i < vec->length) {
        size_t delta = vec->length - i - 1;

        JE_VCALL(&vec->data[i],destroy);

        if(delta) memmove(&vec->data[i], &vec->data[i+1], delta * sizeof(JE_VALUE));
        vec->length--;
    }
}

/**
* Return a pointer to the value at the nth index of the vector.
*/
JE_VALUE* JE_VecGet(const JE_VEC vec, size_t i) {
    JE_VALUE* value = NULL;

    if(i < vec.length) value = &vec.data[i];

    return value;
}

/**
* Return the number of elements in JE_VEC.
*/
size_t JE_VecLength(const JE_VEC vec) {
    return vec.length;
}

/**
* Convert JE_VEC to a C string.  The returned C string must be freed.
*/
char* JE_VecToCstr(const JE_VEC vec) {
    char* cstr = JE_Malloc(_MINSIZE);
    size_t capacity = _MINSIZE;
    size_t clen = 0;

    cstr[clen++] = '[';

    for(size_t i=0; i<vec.length; i++) {
        char* istr = JE_VCALL(&vec.data[i],toQstr);
        size_t ilen = strlen(istr);

        if(i) {
            cstr[clen++] = ',';
            cstr[clen++] = ' ';
        }
        else {
            cstr[clen++] = ' ';
        }

        cstr = JE_Realloc(cstr, clen+ilen+3);
        memcpy(cstr+clen, istr, ilen);
        clen += ilen;

        JE_Free(istr);
    }

    cstr[clen++] = ' ';
    cstr[clen++] = ']';
    cstr[clen++] = '\0';

    return cstr;
}

/**
* Convert JE_VEC to a quoted C string.  The returned quoted C string must be
* freed.
*/
char* JE_VecToQstr(const JE_VEC vec) {
    return JE_VecToCstr(vec);
}
