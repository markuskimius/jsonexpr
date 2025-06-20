#include <stdlib.h>
#include <string.h>
#include "je_map.h"
#include "je_ref.h"
#include "je_str.h"
#include "je_symbols.h"
#include "je_symmap.h"
#include "je_type.h"
#include "je_util.h"
#include "je_value.h"


/* ***************************************************************************
* PRIVATE VARIABLES
*/

static const JE_STR _GLOBAL = { (uint8_t*) "GLOBAL", 6 };
static const JE_STR _UPSCOPE = { (uint8_t*) "UPSCOPE", 7 };


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

/**
* Create a new symbol table.  Set addsyms to 1 to add the default symbols
* (recommended).
*/
JE_MAP* JE_SymmapCreate(int set) {
    JE_MAP* symmap = JE_RefMapAlloc(JE_Symbols(set));

    return symmap;
}

/**
* Create a new scope from an existing parent scope.
*
* With a new scope, any variable that already exists in the parent scope is
* both readable and writable, but any newly created symbol will be created only
* in the new scope.
*/
JE_MAP* JE_SymmapCreateScope(JE_MAP* parent) {
    JE_MAP* symmap = JE_RefMapAlloc(JE_MapCreate(0));
    JE_VALUE* gvalue = JE_MapGet(*parent, _GLOBAL);
    JE_MAP* global = gvalue ? gvalue->data.map : parent;

    JE_MapSet(symmap, JE_StrCreate(JE_Strdup("LOCAL")), JE_ValueRefMap(JE_RefMapDup(symmap)));
    JE_MapSet(symmap, JE_StrCreate(JE_Strdup("GLOBAL")), JE_ValueRefMap(JE_RefMapDup(global)));
    JE_MapSet(symmap, JE_StrCreate(JE_Strdup("UPSCOPE")), JE_ValueRefMap(JE_RefMapDup(parent)));

    return symmap;
}

/**
* Destroy a symbol table.  Only the specified scope will be destroyed.
*/
void JE_SymmapDestroy(JE_MAP* symmap) {
    JE_RefMapFree(symmap);
}

/**
* Set a symbol to a value.
*
* If local is false, and the symbol table already has the specified symbol, the
* value will change in the symbol's existing scope, otherwise a new symbol will
* be created in the local scope.
*
* Once passed into this function, the ownership of both symbol and value will
* change to the symbol table so neither may no longer be modified.  To modify
* the value owned by the symbol table, modify the returned reference to the
* value.
*/
JE_VALUE* JE_SymmapSet(JE_MAP* symmap, JE_STR symbol, JE_VALUE value, int local) {
    JE_VALUE* ref = JE_SymmapGet(*symmap, symbol);

    if(local || !ref) {
        ref = JE_MapSet(symmap, symbol, value);
    }
    else {
        /*
        * Symbol already exists.
        *
        * Destroy the existing symbol value and repliace it with the caller's
        * value.  Also, destroy the symbol from the caller since we can reuse
        * the existing symbol.
        */

        JE_VCALL(ref,destroy);  /* Destroy the existing value */
        *ref = value;           /* Set it to the new value */

        JE_StrDestroy(&symbol); /* Destroy the unused symbol */
    }

    return ref;
}

/**
* Get a reference to a symbol, if any, otherwise return NULL.
*/
JE_VALUE* JE_SymmapGet(const JE_MAP symmap, const JE_STR symbol) {
    JE_VALUE* ref = JE_MapGet(symmap, symbol);
    const JE_MAP* curr = &symmap;

    while(!ref) {
        JE_VALUE* currval = JE_MapGet(*curr, _UPSCOPE);
        if(!currval) break;

        curr = currval->data.map;
        ref = JE_MapGet(*curr, symbol);
    }

    return ref;
}

/**
* Remove a symbol accessible from a scope.  If no such symbol is accessible,
* take no action.
*/
void JE_SymmapRemove(JE_MAP* symmap, const JE_STR symbol) {
    JE_VALUE* ref = JE_MapGet(*symmap, symbol);
    JE_MAP* curr = symmap;

    while(!ref) {
        JE_VALUE* currval = JE_MapGet(*curr, _UPSCOPE);
        if(!currval) break;

        curr = currval->data.map;
        ref = JE_MapGet(*curr, symbol);
    }

    if(ref) JE_MapRemove(curr, symbol);
}
