#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"
#include "util.h"
#include "value.h"
#include "vector.h"


/* ***************************************************************************
* CONSTANTS
*/

#define NDEGREE 256


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static void _setmap(MAP* map, const char* key0, const char* keyi, VALUE* value) {
    int ni = (unsigned)*keyi;
    MAP* nn = map->next[ni];

    /* Terminal node -> assign the value to this node */
    if(ni == 0) {
        if(!map->key) map->key = strdup(key0);
        if(map->value) freevalue(map->value);
        map->value = value;

        return;
    }

    /* Create next node if needed */
    if(!nn) {
        nn = map->next[ni] = newmap();
        nn->prev = map;
    }

    /* Traverse to next node */
    _setmap(nn, key0, keyi+1, value);
}


static MAP* _nextmap(const MAP* map, const char* lastkey, int i) {
    int ni = (unsigned) lastkey[i] + 1;

    /* Next child, if any */
    while(ni < NDEGREE) {
        MAP* nn = map->next[ni];

        if(nn) {
            if(nn->value) return nn;            /* Child node is terminal -> return it */
            else return _nextmap(nn, "\0", 0);  /* Child node is nonterminal -> traverse to it */
        }

        ni++;
    }

    /* Next child of parent */
    if(map->prev && i>=0) {
        return _nextmap(map->prev, lastkey, i-1);
    }

    return NULL;
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

MAP* newmap() {
    MAP* map = calloc(1, sizeof(MAP));

    return map;
}


void freemap(MAP* map) {
    /* Free child nodes */
    for(size_t i=0; i<NDEGREE; i++) {
        MAP* n = map->next[i];

        if(n) freemap(n);
    }

    /* Free this node */
    if(map->key) free((void*) map->key);
    if(map->value) freevalue(map->value);

    free(map);
}


char* astrmap(MAP* map) {
    char* str = calloc(1, strlen("{  }")+1);
    size_t i = 0;

    /* Opening brace */
    str = astrcat(str, "{");

    /* Elements */
    while((map = nextmap(map))) {
        char* kstr = astrencode(map->key);
        char* vstr = strencoded(map->value);

        if(i++ > 0) str = astrcat(str, ",");
        str = astrcat(str, " ");
        str = astrcat(str, kstr);
        str = astrcat(str, ": ");
        str = astrcat(str, vstr);

        free(kstr);
    }

    /* Closing bracket */
    str = astrcat(str, " }");

    return str;
}


void setmap(MAP* map, const char* key, VALUE* value) {
    return _setmap(map, key, key, value);
}


VALUE* getmap(MAP* map, const char* key) {
    int ni = (unsigned)*key;
    MAP* nn = map->next[ni];

    if(ni == 0) return map->value;          /* Terminal node -> return the value at this node */
    else if(nn) return getmap(nn, key+1);   /* Traverse to next node */
    else        return NULL;                /* Not found */
}


MAP* nextmap(const MAP* map) {
    if(map->key) return _nextmap(map, map->key, strlen(map->key));
    else return _nextmap(map, "\0", 0);
}


/* ***************************************************************************
* TEST FUNCTIONS
*/

void _printmap(const MAP* map, char c, int depth) {
    for(int i=0; i<depth; i++) printf("  ");
    printf("[%c] => addr=%p, prev=%p, key=%s, value=%s\n", c, map, map->prev, map->key, strvalue(map->value));

    for(int i=0; i<NDEGREE; i++) {
        const MAP* next = map->next[i];

        if(next) _printmap(next, i, depth+1);
    }
}


void _testmap() {
    MAP* map = newmap();
    const MAP* mapi = map;

    /* Set test */
    setmap(map, "Hello", newstring("world!"));
    setmap(map, "Bye", newstring("cruel world!"));

    /* Get test */
    printf("Bye, %s\n", strencoded(getmap(map, "Bye")));
    printf("Hello, %s\n", strencoded(getmap(map, "Hello")));
    printf("Nosuchkey: %s\n", strencoded(getmap(map, "Nosuchkey")));
    printf("\n");

    /* Print the map */
    // _printmap(map, 0, 0);

    /* Iterator test */
    while((mapi = nextmap(mapi))) {
        printf("%s, %s\n", mapi->key, strencoded(mapi->value));
    }
    printf("\n");

    /* Free test */
    freemap(map);
}
