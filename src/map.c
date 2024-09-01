#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"
#include "util.h"
#include "val.h"


/* ***************************************************************************
* CONSTANTS
*/

#define NDEGREE 256


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static int _mapset(MAP* map, const char* key0, const char* keyi, VAL* val) {
    int ni = *(const unsigned char*)keyi;
    MAP* nn = map->next[ni];
    int incr = 1;

    /* Terminal node -> assign the val to this node */
    if(ni == 0) {
        if(!map->key) map->key = strdup(key0);
        if(map->value) freeval(map->value);
        if(!map->value) incr++;
        map->value = val;

        return incr;
    }

    /* Create next node if needed */
    if(!nn) {
        nn = map->next[ni] = newmap();
        nn->prev = map;
        map->nchildren++;
    }

    /* Traverse to next node */
    return _mapset(nn, key0, keyi+1, val);
}

static int _mapunset(MAP* map, const char* key) {
    int ni = *(const unsigned char*)key;
    MAP* nn = map->next[ni];
    int freed = 0;

    /* Terminal node -> remove the val at this node */
    if(ni == 0) {
        if(map->key) free(map->key);
        if(map->value) freeval(map->value);

        map->key = NULL;
        map->value = NULL;
    }
    /* Traverse to the next node */
    else if(nn) {
        int cfreed = _mapunset(nn, key+1);

        if(cfreed) {
            map->next[ni] = NULL;
            map->nchildren--;
        }
    }

    /* Free this node if it has no children */
    if(map->nchildren == 0) {
        free(map);
        freed = 1;
    }

    return freed;
}


static MAP* _mapnext(MAP* map, const char* lastkey, int i) {
    int ni = *((const unsigned char*)lastkey + i) + 1;

    /* Next child, if any */
    while(ni < NDEGREE) {
        MAP* nn = map->next[ni];

        if(nn) {
            if(nn->value) return nn;            /* Child node is terminal -> return it */
            else return _mapnext(nn, "\0", 0);  /* Child node is nonterminal -> traverse to it */
        }

        ni++;
    }

    /* Next child of parent */
    if(map->prev && i>=0) {
        return _mapnext(map->prev, lastkey, i-1);
    }

    return NULL;
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

MAP* newmap() {
    MAP* map = calloc(1, sizeof(MAP));

    map->count = 1;

    return map;
}


MAP* dupmap(MAP* map) {
    map->count++;

    return map;
}


void freemap(MAP* map) {
    map->count--;

    if(map->count == 0) {
        /* Free child nodes */
        for(size_t i=0; i<NDEGREE; i++) {
            MAP* n = map->next[i];

            if(n) {
                freemap(n);
                map->next[i] = NULL;
            }
        }

        /* Free this node */
        if(map->key) free((void*) map->key);
        if(map->value) freeval(map->value);

        map->key = NULL;
        map->value = NULL;
        map->prev = NULL;
        map->nchildren = 0;
        map->length = 0;
        map->count = 0;
        free(map);
    }
}


void mapset(MAP* map, const char* key, VAL* val) {
    map->length += _mapset(map, key, key, val);
}


void mapunset(MAP* map, const char* key) {
    if(mapget(map, key)) map->length--;

    _mapunset(map, key);
}


VAL* mapget(MAP* map, const char* key) {
    int ni = (unsigned)*key;
    MAP* nn = map->next[ni];

    if(ni == 0) return map->value;          /* Terminal node -> return the val at this node */
    else if(nn) return mapget(nn, key+1);   /* Traverse to next node */
    else        return NULL;                /* Not found */
}


int mapcmp(MAP* map1, MAP* map2) {
    int cmp = 0;

    while(1) {
        map1 = mapnext(map1);
        map2 = mapnext(map2);

        if(map1 && map2) {
            cmp = strcmp(map1->key, map2->key);
            if(cmp == 0) cmp = valcmp(map1->value, map2->value);
        }
        else if(map1) cmp = 1;
        else if(map2) cmp = -1;
        else          break;

        if(cmp != 0) break;
    }

    return cmp;
}


MAP* mapnext(MAP* map) {
    if(map->key) return _mapnext(map, map->key, strlen(map->key));
    else return _mapnext(map, "\0", 0);
}


char* mapastr(MAP* map) {
    char* str = calloc(1, strlen("{  }")+1);
    size_t i = 0;

    /* Opening brace */
    str = astrcat(str, "{");

    /* Elements */
    while((map = mapnext(map))) {
        char* kstr = astrencode(map->key);
        char* vstr = valqstr(map->value);

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


/* ***************************************************************************
* TEST FUNCTIONS
*/

void _printmap(const MAP* map, char c, int depth) {
    for(int i=0; i<depth; i++) printf("  ");
    printf("[%c] => addr=%p, prev=%p, key=%s, val=%s\n", c, map, map->prev, map->key, valstr(map->value));

    for(int i=0; i<NDEGREE; i++) {
        const MAP* next = map->next[i];

        if(next) _printmap(next, i, depth+1);
    }
}


void _testmap() {
    MAP* map = newmap();
    MAP* mapi = map;

    /* Set test */
    mapset(map, "Hello", strval("world!"));
    mapset(map, "Bye", strval("cruel world!"));

    /* Get test */
    printf("Bye, %s\n", valqstr(mapget(map, "Bye")));
    printf("Hello, %s\n", valqstr(mapget(map, "Hello")));
    printf("Nosuchkey: %s\n", valqstr(mapget(map, "Nosuchkey")));
    printf("\n");

    /* Print the map */
    // _printmap(map, 0, 0);

    /* Iterator test */
    while((mapi = mapnext(mapi))) {
        printf("%s, %s\n", mapi->key, valqstr(mapi->value));
    }
    printf("\n");

    /* Free test */
    freemap(map);
}
