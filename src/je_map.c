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
        nn = map->next[ni] = JE_MapNew();
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

JE_MAP* JE_MapNew() {
    JE_MAP* map = JE_Calloc(1, sizeof(JE_MAP));

    map->count = 1;

    return map;
}

JE_MAP* JE_MapDup(JE_MAP* map) {
    map->count++;

    return map;
}

void JE_MapDelete(JE_MAP* map) {
    map->count--;

    if(map->count == 0) {
        /* Free child nodes */
        for(size_t i=0; i<NDEGREE; i++) {
            JE_MAP* n = map->next[i];

            if(n) {
                JE_MapDelete(n);
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

void JE_MapSet(JE_MAP* map, const char* key, JE_VAL* val) {
    map->length += _mapset(map, key, key, val);
}

void JE_MapUnset(JE_MAP* map, const char* key) {
    if(JE_MapGet(map, key)) map->length--;

    _mapunset(map, key);
}

void JE_MapClear(JE_MAP* map) {
    JE_MAP* next = map;

    /* Keep removing the first item */
    while((next = JE_MapNext(map))) {
        JE_MapUnset(map, next->key);
    }
}

JE_VAL* JE_MapGet(JE_MAP* map, const char* key) {
    int ni = (unsigned)*key;
    JE_MAP* nn = map->next[ni];

    if(ni == 0) return map->value;          /* Terminal node -> return the val at this node */
    else if(nn) return JE_MapGet(nn, key+1);   /* Traverse to next node */
    else        return NULL;                /* Not found */
}

int JE_MapCmp(JE_MAP* map1, JE_MAP* map2) {
    int cmp = 0;

    while(1) {
        map1 = JE_MapNext(map1);
        map2 = JE_MapNext(map2);

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

JE_MAP* JE_MapNext(JE_MAP* map) {
    if(map->key) return _mapnext(map, map->key, strlen(map->key));
    else return _mapnext(map, "\0", 0);
}

char* JE_MapToAstr(JE_MAP* map) {
    char* str = JE_Calloc(1, strlen("{  }")+1);
    size_t i = 0;

    /* Opening brace */
    str = je_astrcat(str, "{");

    /* Elements */
    while((map = JE_MapNext(map))) {
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

char* JE_MapKey(JE_MAP* map) {
    return map->key;
}

JE_VAL* JE_MapVal(JE_MAP* map) {
    return map->value;
}


/* ***************************************************************************
* TEST FUNCTIONS
*/

void _JE_MapPrint(const JE_MAP* map, char c, int depth) {
    for(int i=0; i<depth; i++) printf("  ");
    printf("[%c] => addr=%p, prev=%p, key=%s, val=%s\n", c, map, map->prev, map->key, JE_ValToCstr(map->value));

    for(int i=0; i<NDEGREE; i++) {
        const JE_MAP* next = map->next[i];

        if(next) _JE_MapPrint(next, i, depth+1);
    }
}

void _JE_MapTest() {
    JE_MAP* map = JE_MapNew();
    JE_MAP* mapi = map;

    /* Set test */
    JE_MapSet(map, "Hello", JE_ValNewFromCstr("world!"));
    JE_MapSet(map, "Bye", JE_ValNewFromCstr("cruel world!"));

    /* Get test */
    printf("Bye, %s\n", JE_ValToQstr(JE_MapGet(map, "Bye")));
    printf("Hello, %s\n", JE_ValToQstr(JE_MapGet(map, "Hello")));
    printf("Nosuchkey: %s\n", JE_ValToQstr(JE_MapGet(map, "Nosuchkey")));
    printf("\n");

    /* Print the map */
    // _JE_MapPrint(map, 0, 0);

    /* Iterator test */
    while((mapi = JE_MapNext(mapi))) {
        printf("%s, %s\n", mapi->key, JE_ValToQstr(mapi->value));
    }
    printf("\n");

    /* Free test */
    JE_MapDelete(map);
}
