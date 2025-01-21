#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "je_map.h"
#include "je_util.h"
#include "je_val.h"


/* ***************************************************************************
* CONSTANTS
*/

#define NDEGREE 256


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static int _mapset(JE_MAP* map, const char* key0, const char* keyi, JE_VAL* val) {
    int ni = *(const unsigned char*)keyi;
    JE_MAP* nn = map->next[ni];
    int incr = 0;

    /* Terminal node -> assign the val to this node */
    if(ni == 0) {
        if(!map->key) map->key = strdup(key0);
        if(map->value) JE_ValDelete(map->value);
        if(!map->value) incr++;
        map->value = val;

        return incr;
    }

    /* Create next node if needed */
    if(!nn) {
        nn = map->next[ni] = je_newmap();
        nn->prev = map;
        map->nchildren++;
    }

    /* Traverse to next node */
    return _mapset(nn, key0, keyi+1, val);
}

static int _mapunset(JE_MAP* map, const char* key) {
    int ni = *(const unsigned char*)key;
    JE_MAP* nn = map->next[ni];
    int freed = 0;

    /* Terminal node -> remove the val at this node */
    if(ni == 0) {
        if(map->key) JE_Free(map->key);
        if(map->value) JE_ValDelete(map->value);

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

    /* Free this node if it has no children (if it's not the root node) */
    if(map->nchildren == 0 && map->prev) {
        JE_Free(map);
        freed = 1;
    }

    return freed;
}


static JE_MAP* _mapnext(JE_MAP* map, const char* lastkey, int i) {
    int ni = *((const unsigned char*)lastkey + i) + 1;

    /* Next child, if any */
    while(ni < NDEGREE) {
        JE_MAP* nn = map->next[ni];

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

JE_MAP* je_newmap() {
    JE_MAP* map = JE_Calloc(1, sizeof(JE_MAP));

    map->count = 1;

    return map;
}


JE_MAP* je_dupmap(JE_MAP* map) {
    map->count++;

    return map;
}


void je_freemap(JE_MAP* map) {
    map->count--;

    if(map->count == 0) {
        /* Free child nodes */
        for(size_t i=0; i<NDEGREE; i++) {
            JE_MAP* n = map->next[i];

            if(n) {
                je_freemap(n);
                map->next[i] = NULL;
            }
        }

        /* Free this node */
        if(map->key) JE_Free((void*) map->key);
        if(map->value) JE_ValDelete(map->value);

        map->key = NULL;
        map->value = NULL;
        map->prev = NULL;
        map->nchildren = 0;
        map->length = 0;
        map->count = 0;
        JE_Free(map);
    }
}


void je_mapset(JE_MAP* map, const char* key, JE_VAL* val) {
    map->length += _mapset(map, key, key, val);
}


void je_mapunset(JE_MAP* map, const char* key) {
    if(je_mapget(map, key)) map->length--;

    _mapunset(map, key);
}


void je_mapclear(JE_MAP* map) {
    JE_MAP* next = map;

    /* Keep removing the first item */
    while((next = je_mapnext(map))) {
        je_mapunset(map, next->key);
    }
}


JE_VAL* je_mapget(JE_MAP* map, const char* key) {
    int ni = (unsigned)*key;
    JE_MAP* nn = map->next[ni];

    if(ni == 0) return map->value;          /* Terminal node -> return the val at this node */
    else if(nn) return je_mapget(nn, key+1);   /* Traverse to next node */
    else        return NULL;                /* Not found */
}


int je_mapcmp(JE_MAP* map1, JE_MAP* map2) {
    int cmp = 0;

    while(1) {
        map1 = je_mapnext(map1);
        map2 = je_mapnext(map2);

        if(map1 && map2) {
            cmp = strcmp(map1->key, map2->key);
            if(cmp == 0) cmp = JE_ValCmp(map1->value, map2->value);
        }
        else if(map1) cmp = 1;
        else if(map2) cmp = -1;
        else          break;

        if(cmp != 0) break;
    }

    return cmp;
}


JE_MAP* je_mapnext(JE_MAP* map) {
    if(map->key) return _mapnext(map, map->key, strlen(map->key));
    else return _mapnext(map, "\0", 0);
}


char* je_mapastr(JE_MAP* map) {
    char* str = JE_Calloc(1, strlen("{  }")+1);
    size_t i = 0;

    /* Opening brace */
    str = je_astrcat(str, "{");

    /* Elements */
    while((map = je_mapnext(map))) {
        char* kstr = je_astrencode(map->key);
        const char* vstr = JE_ValToQstr(map->value);

        if(i++ > 0) str = je_astrcat(str, ",");
        str = je_astrcat(str, " ");
        str = je_astrcat(str, kstr);
        str = je_astrcat(str, ": ");
        str = je_astrcat(str, vstr);

        JE_Free(kstr);
    }

    /* Closing bracket */
    str = je_astrcat(str, " }");

    return str;
}

char* je_mapkey(JE_MAP* map) {
    return map->key;
}

JE_VAL* je_mapval(JE_MAP* map) {
    return map->value;
}


/* ***************************************************************************
* TEST FUNCTIONS
*/

void _je_printmap(const JE_MAP* map, char c, int depth) {
    for(int i=0; i<depth; i++) printf("  ");
    printf("[%c] => addr=%p, prev=%p, key=%s, val=%s\n", c, map, map->prev, map->key, JE_ValToCstr(map->value));

    for(int i=0; i<NDEGREE; i++) {
        const JE_MAP* next = map->next[i];

        if(next) _je_printmap(next, i, depth+1);
    }
}


void _je_testmap() {
    JE_MAP* map = je_newmap();
    JE_MAP* mapi = map;

    /* Set test */
    je_mapset(map, "Hello", JE_ValNewFromCstr("world!"));
    je_mapset(map, "Bye", JE_ValNewFromCstr("cruel world!"));

    /* Get test */
    printf("Bye, %s\n", JE_ValToQstr(je_mapget(map, "Bye")));
    printf("Hello, %s\n", JE_ValToQstr(je_mapget(map, "Hello")));
    printf("Nosuchkey: %s\n", JE_ValToQstr(je_mapget(map, "Nosuchkey")));
    printf("\n");

    /* Print the map */
    // _je_printmap(map, 0, 0);

    /* Iterator test */
    while((mapi = je_mapnext(mapi))) {
        printf("%s, %s\n", mapi->key, JE_ValToQstr(mapi->value));
    }
    printf("\n");

    /* Free test */
    je_freemap(map);
}
