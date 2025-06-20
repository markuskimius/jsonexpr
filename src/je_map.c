#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "je_map.h"
#include "je_str.h"
#include "je_type.h"
#include "je_util.h"
#include "je_value.h"


/* ***************************************************************************
* CONSTANTS
*/

#define _MINSIZE 32


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static void _resize(JE_MAP* map, size_t nmemb) {
    map->key = JE_Realloc(map->key, sizeof(JE_STR) * nmemb);
    map->value = JE_Realloc(map->value, sizeof(JE_VALUE) * nmemb);
}

static ssize_t _find(const JE_MAP map, const JE_STR key) {
    ssize_t i = 0;
    ssize_t j = map.size;
    ssize_t k = (i+j)/2;

    while(i<j) {
        int64_t cmp = JE_StrCompare(key, map.key[k]);

        if(cmp == 0) return k;
        if(cmp < 0) j = k;
        if(cmp > 0) i = k+1;

        k = (i+j)/2;
    }

    return ~k;
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

/**
* Create a new JE_MAP, optionally specifying the initial capacity.  0 can be
* passed instead of the capacity to default to a reasonable initial capacity.
*/
JE_MAP JE_MapCreate(size_t capacity) {
    JE_MAP map;

    map.key = JE_Malloc(sizeof(JE_STR) * (capacity ? capacity : _MINSIZE));
    map.value = JE_Malloc(sizeof(JE_VALUE) * (capacity ? capacity : _MINSIZE));
    map.size = 0;

    return map;
}

/**
* Return a clone of map.
*/
JE_MAP JE_MapClone(const JE_MAP map) {
    JE_MAP dup;

    dup.key = JE_Malloc(sizeof(JE_STR) * (map.size<_MINSIZE ? _MINSIZE : map.size));
    dup.value = JE_Malloc(sizeof(JE_VALUE) * (map.size<_MINSIZE ? _MINSIZE : map.size));
    dup.size = map.size;

    for(size_t i=0; i<map.size; i++) dup.key[i] = JE_StrClone(map.key[i]);
    for(size_t i=0; i<map.size; i++) dup.value[i] = JE_VCALL(&map.value[i],clone);

    return dup;
}

/**
* Concatenate two JE_MAP's, return a new JE_MAP.
*/
JE_MAP JE_MapPlusMap(const JE_MAP x, const JE_MAP y) {
    JE_MAP map = JE_MapClone(x);

    for(size_t i=0; i<y.size; i++) JE_MapSet(&map, JE_StrClone(y.key[i]), JE_VCALL(&y.value[i],clone));

    return map;
}

/**
* Compare x to y.  Return a value less than, equal to, or greater than 0 if
* x is less than, equal to, or greater than y, respectively.
*/
int64_t JE_MapCompare(const JE_MAP x, const JE_MAP y) {
    size_t size = x.size<y.size ? x.size : y.size;

    for(size_t i=0; i<size; i++) {
        int64_t cmp = 0;

        cmp = JE_StrCompare(x.key[i], y.key[i]);
        if(cmp) return cmp;

        cmp = JE_VCALL(&x.value[i],compare,y.value[i]);
        if(cmp) return cmp;
    }

    return x.size - y.size;
}

/**
* Concatenate y to x.
*/
void JE_MapPlusAssnMap(JE_MAP* x, const JE_MAP y) {
    _resize(x, x->size + y.size);

    for(size_t i=0; i<y.size; i++) JE_MapSet(x, JE_StrClone(y.key[i]), JE_VCALL(&y.value[i],clone));
}

/**
* Destroy map.
*/
void JE_MapDestroy(JE_MAP* map) {
    for(size_t i=0; i<map->size; i++) JE_StrDestroy(&map->key[i]);
    for(size_t i=0; i<map->size; i++) JE_VCALL(&map->value[i],destroy);

    JE_Free(map->key);
    JE_Free(map->value);

    memset(map, 0, sizeof(JE_MAP));
}

/**
* Map a key to a value.
*
* Once added into map, key and val may no longer be operated on by the caller.
* If val needs to be modified after the set, modify the returned pointer
* instead.
*
* If the key already maps to a value, the old key and the old value are destroyed.
*/
JE_VALUE* JE_MapSet(JE_MAP* map, JE_STR key, JE_VALUE val) {
    ssize_t i = _find(*map, key);
    size_t nshift = 0;

    if(i >= 0) {
        JE_StrDestroy(&map->key[i]);
        JE_VCALL(&map->value[i],destroy);
    }
    else {
        i = ~i;
        nshift = map->size - i;

        /* Allocate space */
        map->size++;
        map->key = JE_Realloc(map->key, sizeof(JE_STR) * map->size);
        map->value = JE_Realloc(map->value, sizeof(JE_VALUE) * map->size);

        /* Make space for the new item */
        memmove(&map->key[i+1], &map->key[i], sizeof(JE_STR) * nshift);
        memmove(&map->value[i+1], &map->value[i], sizeof(JE_VALUE) * nshift);
    }

    /* Set */
    map->key[i] = key;
    map->value[i] = val;

    return &map->value[i];
}

JE_VALUE* JE_MapGet(const JE_MAP map, const JE_STR key) {
    ssize_t i = _find(map, key);
    JE_VALUE* value = NULL;

    if(i >= 0) value = &map.value[i];

    return value;
}

void JE_MapRemove(JE_MAP* map, const JE_STR key) {
    ssize_t i = _find(*map, key);

    if(i >= 0) {
        size_t nshift = map->size - i - 1;

        /* Destroy */
        JE_StrDestroy(&map->key[i]);
        JE_VCALL(&map->value[i],destroy);

        /* Shift */
        if(nshift) {
            memmove(&map->key[i], &map->key[i+1], sizeof(JE_STR) * nshift);
            memmove(&map->value[i], &map->value[i+1], sizeof(JE_VALUE) * nshift);
        }

        /* Update size */
        map->size--;
    }
}

/**
* Return the number of elements in JE_MAP.
*/
size_t JE_MapSize(const JE_MAP map) {
    return map.size;
}

/**
* Convert JE_MAP to a C string.  The returned C string must be freed.
*/
char* JE_MapToCstr(const JE_MAP map) {
    char* cstr = JE_Malloc(_MINSIZE);
    size_t capacity = _MINSIZE;
    size_t clen = 0;

    cstr[clen++] = '{';

    for(size_t i=0; i<map.size; i++) {
        char* kstr = JE_StrToQstr(map.key[i]);
        char* vstr = JE_VCALL(&map.value[i],toQstr);
        size_t klen = strlen(kstr);
        size_t vlen = strlen(vstr);

        if(i) cstr[clen++] = ',';
        cstr[clen++] = ' ';

        /* allocate memory */
        cstr = JE_Realloc(cstr, clen+klen+vlen+5);

        /* key */
        memcpy(cstr+clen, kstr, klen);
        clen += klen;

        /* : */
        cstr[clen++] = ':';
        cstr[clen++] = ' ';

        /* value */
        memcpy(cstr+clen, vstr, vlen);
        clen += vlen;

        JE_Free(kstr);
        JE_Free(vstr);
    }

    cstr[clen++] = ' ';
    cstr[clen++] = '}';
    cstr[clen++] = '\0';

    return cstr;
}

/**
* Convert JE_MAP to a quoted C string.  The returned quoted C string must be
* freed.
*/
char* JE_MapToQstr(const JE_MAP map) {
    return JE_MapToCstr(map);
}
