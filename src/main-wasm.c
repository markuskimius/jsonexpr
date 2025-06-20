#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "je.h"
#include "je_util.h"


/* ***************************************************************************
* CONSTANTS
*/

#define _LISTSIZE 32


/* ***************************************************************************
* MACROS
*/

#define _EXPORT(name) __attribute__((export_name(#name)))


/* ***************************************************************************
* PRIVATE TYPES
*/

typedef struct _ASTLIST _ASTLIST;
typedef struct _VALUELIST _VALUELIST;

struct _ASTLIST {
    JE_AST item[_LISTSIZE];
    int nref[_LISTSIZE];

    size_t count;
    _ASTLIST* next;
};

struct _VALUELIST {
    JE_VALUE item[_LISTSIZE];
    int nref[_LISTSIZE];

    size_t count;
    _VALUELIST* next;
};


/* ***************************************************************************
* PRIVATE VARIABLES
*/

static _ASTLIST _astlist;
static _VALUELIST _valuelist;


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static _ASTLIST* _FindAstNode(const JE_AST* ast) {
    _ASTLIST* node = &_astlist;

    while(node && (ast < node->item || node->item+_LISTSIZE <= ast)) node = node->next;

    return node;
}

static _VALUELIST* _FindValueNode(const JE_VALUE* value) {
    _VALUELIST* node = &_valuelist;

    while(node && (value < node->item || node->item+_LISTSIZE <= value)) node = node->next;

    return node;
}

static JE_AST* _AstAlloc() {
    _ASTLIST* node = &_astlist;
    JE_AST* ref = NULL;

    /* Find the first free node */
    while(node->count >= _LISTSIZE && node->next) node = node->next;

    /* Allocate a new node if no block is free */
    if(node->count >= _LISTSIZE) {
        assert(node->next == NULL);

        node->next = JE_Calloc(1, sizeof(_ASTLIST));
        node = node->next;
    }

    /* Find the free slot */
    for(size_t i=0; i<_LISTSIZE; i++) {
        if(node->nref[i] == 0) {
            ref = &node->item[i];   /* Reference the free slot */
            node->nref[i]++;        /* Increment the reference counter */
            node->count++;          /* Increment the used slot counter */
            break;
        }
    }

    assert(ref);

    return ref;
}

static JE_VALUE* _ValueAlloc() {
    _VALUELIST* node = &_valuelist;
    JE_VALUE* ref = NULL;

    /* Find the first free node */
    while(node->count >= _LISTSIZE && node->next) node = node->next;

    /* Allocate a new node if no block is free */
    if(node->count >= _LISTSIZE) {
        assert(node->next == NULL);

        node->next = JE_Calloc(1, sizeof(_VALUELIST));
        node = node->next;
    }

    /* Find the free slot */
    for(size_t i=0; i<_LISTSIZE; i++) {
        if(node->nref[i] == 0) {
            ref = &node->item[i];   /* Reference the free slot */
            node->nref[i]++;        /* Increment the reference counter */
            node->count++;          /* Increment the used slot counter */
            break;
        }
    }

    assert(ref);

    return ref;
}

static void _AstFree(JE_AST* ast) {
    _ASTLIST* node = _FindAstNode(ast);
    size_t i = ast - node->item;

    assert(node);
    assert(node->count > 0);
    assert(i < _LISTSIZE);
    assert(ast == &node->item[i]);

    /* Decrement the reference counter */
    node->nref[i]--;

    /* Free if no references */
    if(node->nref[i] <= 0) {
        JE_AstDestroy(ast);

        node->count--;
    }
}

static void _ValueFree(JE_VALUE* value) {
    _VALUELIST* node = _FindValueNode(value);
    size_t i = value - node->item;

    assert(node);
    assert(node->count > 0);
    assert(i < _LISTSIZE);
    assert(value == &node->item[i]);

    /* Decrement the reference counter */
    node->nref[i]--;

    /* Free if no references */
    if(node->nref[i] <= 0) {
        JE_VCALL(value,destroy);

        node->count--;
    }
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

int main(int argc, const char* argv[]) {
    printf("Please call parse() and asteval() directly.\n");

    return 0;
}


/* ***************************************************************************
* PUBLIC FUNCTIONS - AST
*/

_EXPORT(parse)
JE_AST* parse(const char* code) {
    JE_AST* ast = _AstAlloc();

    *ast = JE_Parse(code);

    return ast;
}

_EXPORT(asteval)
JE_VALUE* asteval(const JE_AST* ast, JE_MAP* symmap) {
    JE_VALUE* value = _ValueAlloc();

    *value = JE_AstEval(*ast, symmap);

    return value;
}

_EXPORT(astfree)
void astfree(JE_AST* ast) {
    _AstFree(ast);
}


/* ***************************************************************************
* PUBLIC FUNCTIONS - SYMMAP
*/

_EXPORT(newsym)
JE_MAP* newsym(JE_MAP* parent) {
    JE_MAP* symmap = parent ? JE_SymmapCreateScope(parent) : JE_SymmapCreate(JE_SYMBOLS_DEFAULT);

    return symmap;
}

_EXPORT(symget)
const JE_VALUE* symget(const JE_MAP* symmap, const char* key) {
    JE_STR skey = JE_StrCreate((char*) key);  /* Temporarily steal key */
    JE_VALUE* result = JE_SymmapGet(*symmap, skey);

    /* Do not destroy skey since key was stolen */

    return result;
}

_EXPORT(symset)
JE_VALUE* symset(JE_MAP* symmap, char* key, JE_VALUE* value, int local) {
    JE_STR skey = JE_StrCreate(key);
    JE_VALUE* result = JE_SymmapSet(symmap, skey, *value, local);

    /* key, value are now in symmap, do not free */

    return result;
}

_EXPORT(symunset)
void symunset(JE_MAP* symmap, const char* key) {
    JE_STR skey = JE_StrCreate((char*) key);  /* Temporarily steal key */

    JE_SymmapRemove(symmap, skey);

    /* Do not destroy skey since key was stolen */
}

_EXPORT(symfree)
void symfree(JE_MAP* symmap) {
    JE_SymmapDestroy(symmap);
}


/* ***************************************************************************
* PUBLIC FUNCTIONS - VALUE
*/

_EXPORT(nullval)
JE_VALUE* nullval() {
    JE_VALUE* value = _ValueAlloc();

    *value = JE_ValueNul();

    return value;
}

_EXPORT(boolval)
JE_VALUE* boolval(int64_t tru) {
    JE_VALUE* value = _ValueAlloc();

    *value = JE_ValueTru(tru);

    return value;
}

_EXPORT(i64val)
JE_VALUE* i64val(int64_t i64) {
    JE_VALUE* value = _ValueAlloc();

    *value = JE_ValueI64(i64);

    return value;
}

_EXPORT(f64val)
JE_VALUE* f64val(double f64) {
    JE_VALUE* value = _ValueAlloc();

    *value = JE_ValueF64(f64);

    return value;
}

_EXPORT(strval)
JE_VALUE* strval(char* cstr) {
    JE_VALUE* value = _ValueAlloc();

    *value = JE_ValueStr(JE_StrCreate(cstr));

    return value;
}

_EXPORT(valdup)
JE_VALUE* valdup(JE_VALUE* value) {
    JE_VALUE* clone = _ValueAlloc();

    *clone = JE_VCALL(value,clone);

    return clone;
}

_EXPORT(valtype)
const char* valtype(JE_VALUE* value) {
    return JE_VCALL(value,getTypeCstr);
}

_EXPORT(valfree)
void valfree(JE_VALUE* value) {
    _ValueFree(value);
}

_EXPORT(valtobool)
int64_t valtobool(JE_VALUE* value) {
    int64_t result = 0;

    if(value->type == &JE_typeTru) {
        result = value->data.tru;
    }

    return result;
}

_EXPORT(valtoi64)
int64_t valtoi64(JE_VALUE* value) {
    int64_t result = 0;

    if(value->type == &JE_typeI64) {
        result = value->data.i64;
    }

    return result;
}

_EXPORT(valtof64)
double valtof64(JE_VALUE* value) {
    double result = 0;

    if(value->type == &JE_typeF64) {
        result = value->data.f64;
    }

    return result;
}

_EXPORT(valtocstr)
char* valtocstr(JE_VALUE* value) {
    return JE_VCALL(value,toCstr);
}

_EXPORT(valtoqstr)
char* valtoqstr(JE_VALUE* value) {
    return JE_VCALL(value,toQstr);
}


/* ***************************************************************************
* PUBLIC FUNCTIONS - MAP
*/

_EXPORT(mapval)
JE_VALUE* mapval() {
    JE_VALUE* value = _ValueAlloc();

    *value = JE_ValueMap(JE_MapCreate(0));

    return value;
}

_EXPORT(vecval)
JE_VALUE* vecval() {
    JE_VALUE* value = _ValueAlloc();

    *value = JE_ValueVec(JE_VecCreate(0));

    return value;
}

_EXPORT(maplen)
int64_t maplen(const JE_VALUE* map) {
    JE_VALUE length = JE_VCALL(map,length);

    return length.data.i64;
}

_EXPORT(mapget)
const JE_VALUE* mapget(const JE_VALUE* map, const char* key) {
    JE_VALUE* result = NULL;

    if(map->type == &JE_typeMap) {
        JE_STR skey = JE_StrCreate((char*) key);  /* Temporarily steal key */

        result = JE_MapGet(*map->data.map, skey);

        /* Do not destroy skey since key was stolen */
    }

    return result;
}

_EXPORT(mapset)
JE_VALUE* mapset(JE_VALUE* map, char* key, JE_VALUE* value) {
    JE_STR skey = JE_StrCreate(key);
    JE_VALUE* result = NULL;

    if(map->type == &JE_typeMap) {
        result = JE_MapSet(map->data.map, skey, *value);
    }
    else {
        JE_StrDestroy(&skey);
    }

    return result;
}

_EXPORT(mapunset)
void mapunset(JE_VALUE* map, const char* key) {
    JE_STR skey = JE_StrCreate((char*) key);  /* Temporarily steal key */

    JE_MapRemove(map->data.map, skey);

    /* Do not destroy skey since key was stolen */
}

_EXPORT(mapbegin)
int64_t mapbegin(const JE_VALUE* map) {
    return 0;
}

_EXPORT(mapend)
int64_t mapend(const JE_VALUE* map) {
    return maplen(map);
}

_EXPORT(mapnext)
int64_t mapnext(const JE_VALUE* map, int64_t iter) {
    return iter+1;
}

_EXPORT(mapgetkey)
const char* mapgetkey(const JE_VALUE* map, int64_t iter) {
    const char* key = NULL;

    if(iter < map->data.map->size) {
        key = (const char*) map->data.map->key[iter].data;
    }

    return key;
}

_EXPORT(mapgetval)
const JE_VALUE* mapgetval(const JE_VALUE* map, int64_t iter) {
    const JE_VALUE* value = NULL;

    if(iter < map->data.map->size) {
        value = &map->data.map->value[iter];
    }

    return value;
}


/* ***************************************************************************
* PUBLIC FUNCTIONS - VEC
*/

_EXPORT(veclen)
int64_t veclen(JE_VALUE* vec) {
    JE_VALUE length = JE_VCALL(vec,length);

    return length.data.i64;
}

_EXPORT(vecget)
const JE_VALUE* vecget(const JE_VALUE* vec, int64_t index) {
    JE_VALUE* result = NULL;

    if(vec->type == &JE_typeVec) {
        result = JE_VecGet(*vec->data.vec, index);
    }

    return result;
}

_EXPORT(vecset)
JE_VALUE* vecset(JE_VALUE* vec, int64_t index, JE_VALUE* value) {
    JE_VALUE* result = NULL;

    if(vec->type == &JE_typeVec) {
        result = JE_VecSet(vec->data.vec, index, *value);
    }

    return result;
}

_EXPORT(vecpush)
JE_VALUE* vecpush(JE_VALUE* vec, JE_VALUE* value) {
    return JE_VecPush(vec->data.vec, *value);
}

_EXPORT(vecpop)
JE_VALUE* vecpop(JE_VALUE* vec) {
    JE_VALUE* value = _ValueAlloc();

    *value = JE_VecPop(vec->data.vec);

    return value;
}

_EXPORT(vecunset)
void vecunset(JE_VALUE* vec, int64_t index) {
    JE_VecRemove(vec->data.vec, index);
}
