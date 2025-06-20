#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "je_ref.h"
#include "je_util.h"


/* ***************************************************************************
* CONSTANTS
*/

#define _LISTSIZE 32


/* ***************************************************************************
* PRIVATE TYPES
*/

typedef struct _MAPLIST _MAPLIST;
typedef struct _VECLIST _VECLIST;
typedef struct _FUNLIST _FUNLIST;

struct _MAPLIST {
    JE_MAP map[_LISTSIZE];
    int nref[_LISTSIZE];

    size_t count;
    _MAPLIST* next;
};

struct _VECLIST {
    JE_VEC vec[_LISTSIZE];
    int nref[_LISTSIZE];

    size_t count;
    _VECLIST* next;
};

struct _FUNLIST {
    JE_FUN fun[_LISTSIZE];
    int nref[_LISTSIZE];

    size_t count;
    _FUNLIST* next;
};


/* ***************************************************************************
* PRIVATE VARIABLES
*/

static _MAPLIST _maplist;
static _VECLIST _veclist;
static _FUNLIST _funlist;


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static _MAPLIST* _FindMapNode(const JE_MAP* map) {
    _MAPLIST* node = &_maplist;

    while(node && (map < node->map || node->map+_LISTSIZE <= map)) node = node->next;

    return node;
}

static _VECLIST* _FindVecNode(const JE_VEC* vec) {
    _VECLIST* node = &_veclist;

    while(node && (vec < node->vec || node->vec+_LISTSIZE <= vec)) node = node->next;

    return node;
}

static _FUNLIST* _FindFunNode(const JE_FUN* fun) {
    _FUNLIST* node = &_funlist;

    while(node && (fun < node->fun || node->fun+_LISTSIZE <= fun)) node = node->next;

    return node;
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_MAP* JE_RefMapAlloc(JE_MAP map) {
    _MAPLIST* node = &_maplist;
    JE_MAP* ref = NULL;

    /* Find the first free node */
    while(node->count >= _LISTSIZE && node->next) node = node->next;

    /* Allocate a new node if no block is free */
    if(node->count >= _LISTSIZE) {
        assert(node->next == NULL);

        node->next = JE_Calloc(1, sizeof(_MAPLIST));
        node = node->next;
    }

    /* Find the free slot */
    for(size_t i=0; i<_LISTSIZE; i++) {
        if(node->nref[i] == 0) {
            ref = &node->map[i];    /* Reference the free slot */
            *ref = map;             /* Move the map into the slot */
            node->nref[i]++;        /* Increment the reference counter */
            node->count++;          /* Increment the used slot counter */
            break;
        }
    }

    assert(ref);

    return ref;
}

JE_VEC* JE_RefVecAlloc(JE_VEC vec) {
    _VECLIST* node = &_veclist;
    JE_VEC* ref = NULL;

    /* Find the first free node */
    while(node->count >= _LISTSIZE && node->next) node = node->next;

    /* Allocate a new node if no block is free */
    if(node->count >= _LISTSIZE) {
        assert(node->next == NULL);

        node->next = JE_Calloc(1, sizeof(_VECLIST));
        node = node->next;
    }

    /* Find the free slot */
    for(size_t i=0; i<_LISTSIZE; i++) {
        if(node->nref[i] == 0) {
            ref = &node->vec[i];    /* Reference the free slot */
            *ref = vec;             /* Move the vec into the slot */
            node->nref[i]++;        /* Increment the reference counter */
            node->count++;          /* Increment the used slot counter */
            break;
        }
    }

    assert(ref);

    return ref;
}

JE_FUN* JE_RefFunAlloc(JE_FUN fun) {
    _FUNLIST* node = &_funlist;
    JE_FUN* ref = NULL;

    /* Find the first free node */
    while(node->count >= _LISTSIZE && node->next) node = node->next;

    /* Allocate a new node if no block is free */
    if(node->count >= _LISTSIZE) {
        assert(node->next == NULL);

        node->next = JE_Calloc(1, sizeof(_FUNLIST));
        node = node->next;
    }

    /* Find the free slot */
    for(size_t i=0; i<_LISTSIZE; i++) {
        if(node->nref[i] == 0) {
            ref = &node->fun[i];    /* Reference the free slot */
            *ref = fun;             /* Move the fun into the slot */
            node->nref[i]++;        /* Increment the reference counter */
            node->count++;          /* Increment the used slot counter */
            break;
        }
    }

    assert(ref);

    return ref;
}

JE_MAP* JE_RefMapDup(const JE_MAP* map) {
    _MAPLIST* node = _FindMapNode(map);
    size_t i = map - node->map;

    assert(node);
    assert(node->count > 0);
    assert(i < _LISTSIZE);
    assert(map == &node->map[i]);

    /* Increment the reference counter */
    node->nref[i]++;

    return &node->map[i];
}

JE_VEC* JE_RefVecDup(const JE_VEC* vec) {
    _VECLIST* node = _FindVecNode(vec);
    size_t i = vec - node->vec;

    assert(node);
    assert(node->count > 0);
    assert(i < _LISTSIZE);
    assert(vec == &node->vec[i]);

    /* Increment the reference counter */
    node->nref[i]++;

    return &node->vec[i];
}

JE_FUN* JE_RefFunDup(const JE_FUN* fun) {
    _FUNLIST* node = _FindFunNode(fun);
    size_t i = fun - node->fun;

    assert(node);
    assert(node->count > 0);
    assert(i < _LISTSIZE);
    assert(fun == &node->fun[i]);

    /* Increment the reference counter */
    node->nref[i]++;

    return &node->fun[i];
}

void JE_RefMapFree(JE_MAP* map) {
    _MAPLIST* node = _FindMapNode(map);
    size_t i = map - node->map;

    assert(node);
    assert(node->count > 0);
    assert(i < _LISTSIZE);
    assert(map == &node->map[i]);

    /* Decrement the reference counter */
    node->nref[i]--;

    /* Free if no references */
    if(node->nref[i] <= 0) {
        JE_MapDestroy(map);

        node->count--;
    }
}

void JE_RefVecFree(JE_VEC* vec) {
    _VECLIST* node = _FindVecNode(vec);
    size_t i = vec - node->vec;

    assert(node);
    assert(node->count > 0);
    assert(i < _LISTSIZE);
    assert(vec == &node->vec[i]);
    assert(node->nref[i] > 0);

    /* Decrement the reference counter */
    node->nref[i]--;

    /* Free if no references */
    if(node->nref[i] <= 0) {
        JE_VecDestroy(vec);

        node->count--;
    }
}

void JE_RefFunFree(JE_FUN* fun) {
    _FUNLIST* node = _FindFunNode(fun);
    size_t i = fun - node->fun;

    assert(node);
    assert(node->count > 0);
    assert(i < _LISTSIZE);
    assert(fun == &node->fun[i]);

    /* Decrement the reference counter */
    node->nref[i]--;

    /* Free if no references */
    if(node->nref[i] <= 0) {
        JE_FunDestroy(fun);

        node->count--;
    }
}
