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

static int _setmap(MAP* map, const char* key0, const char* keyi, VALUE* value) {
    int ni = *(const unsigned char*)keyi;
    MAP* nn = map->next[ni];
    int incr = 1;

    /* Terminal node -> assign the value to this node */
    if(ni == 0) {
        if(!map->key) map->key = strdup(key0);
        if(map->value) freevalue(map->value);
        if(!map->value) incr++;
        map->value = value;

        return incr;
    }

    /* Create next node if needed */
    if(!nn) {
        nn = map->next[ni] = newmap();
        nn->prev = map;
        map->nchildren++;
    }

    /* Traverse to next node */
    return _setmap(nn, key0, keyi+1, value);
}

static int _unsetmap(MAP* map, const char* key) {
    int ni = *(const unsigned char*)key;
    MAP* nn = map->next[ni];
    int freed = 0;

    /* Terminal node -> remove the value at this node */
    if(ni == 0) {
        if(map->key) free(map->key);
        if(map->value) freevalue(map->value);

        map->key = NULL;
        map->value = NULL;
    }
    /* Traverse to the next node */
    else if(nn) {
        int cfreed = _unsetmap(nn, key+1);

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


static MAP* _nextmap(const MAP* map, const char* lastkey, int i) {
    int ni = *((const unsigned char*)lastkey + i) + 1;

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
    map->length += _setmap(map, key, key, value);
}


void unsetmap(MAP* map, const char* key) {
    if(getmap(map, key)) map->length--;

    _unsetmap(map, key);
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
    printf("[%c] => addr=%p, prev=%p, key=%s, value=%s\n", c, map, map->prev, map->key, strdecoded(map->value));

    for(int i=0; i<NDEGREE; i++) {
        const MAP* next = map->next[i];

        if(next) _printmap(next, i, depth+1);
    }
}


void _testmap() {
    MAP* map = newmap();
    const MAP* mapi = map;

    /* Set test */
    setmap(map, "Hello", strvalue("world!"));
    setmap(map, "Bye", strvalue("cruel world!"));

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


int cmpmap(MAP* map1, MAP* map2) {
    int cmp = 0;

    while(1) {
        map1 = nextmap(map1);
        map2 = nextmap(map2);

        if(map1 && map2) {
            cmp = strcmp(map1->key, map2->key);
            if(cmp == 0) cmp = cmpvalue(map1->value, map2->value);
        }
        else if(map1) cmp = 1;
        else if(map2) cmp = -1;
        else          break;

        if(cmp != 0) break;
    }

    return cmp;
}
