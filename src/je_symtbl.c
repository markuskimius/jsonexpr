#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "je_builtin.h"
#include "je_map.h"
#include "je_symtbl.h"
#include "je_val.h"
#include "je_util.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

static JE_SYMTBL* _newtable(JE_SYMTBL* parent, int isbuiltin) {
    JE_SYMTBL* table = JE_Calloc(1, sizeof(JE_SYMTBL));

    table->symbols = JE_MapNew();
    table->symval = JE_ValNewFromMap(JE_MapDup(table->symbols));

    /* Built-in */
    if(!parent && isbuiltin) {
        JE_MAP* bi = JE_BuiltinFns();

        while((bi = JE_MapNext(bi))) {
            JE_MapSet(table->symbols, bi->key, bi->value);
        }
    }

    /* Global */
    else if(!parent && !isbuiltin) {
        table->builtin = _newtable(NULL, 1);
        table->parent = table->builtin;
        table->global = table;
    }

    /* Regular table */
    else {
        table->builtin = parent->builtin;
        table->global = parent->global;
        table->parent = parent;
    }

    /* Add UPSCOPE */
    if(table->parent) {
        JE_SymtblDup(table->parent);
        JE_MapSet(table->symbols, "UPSCOPE", JE_ValNewFromMap(JE_MapDup(table->parent->symbols)));
    }

    return JE_SymtblDup(table);
}

JE_SYMTBL* JE_SymtblNew(JE_SYMTBL* parent) {
    return _newtable(parent, 0);
}

JE_SYMTBL* JE_SymtblDup(JE_SYMTBL* table) {
    table->count++;

    if(table->parent) {
        JE_SymtblDup(table->parent);
    }

    return table;
}

void JE_SymtblDelete(JE_SYMTBL* table) {
    table->count--;

    /* Reduce parent counter */
    if(table->parent) {
        JE_SymtblDelete(table->parent);
    }

    /* Free this table if it's no longer used */
    if(table->count == 0) {
        JE_ValDelete(table->symval);
        JE_MapDelete(table->symbols);
        free(table);
    }
}

void JE_SymtblSet(JE_SYMTBL* table, const char* name, JE_VAL* val, int localonly) {
    /* Set at this level if the symbol exists at this level */
    if(JE_MapGet(table->symbols, name)) {
        JE_MapSet(table->symbols, name, val);
    }
    /* Set in parent if the symbol exists at the higher level */
    else if(!localonly && table->parent && JE_SymtblGet(table->parent, name)) {
        JE_SymtblSet(table->parent, name, val, localonly);
    }
    /* Otherwise set at this level */
    else {
        JE_MapSet(table->symbols, name, val);
    }
}

void JE_SymtblUnset(JE_SYMTBL* table, const char* name) {
    /* Unset at this level if the symbol exists at this level */
    if(JE_MapGet(table->symbols, name)) {
        JE_MapUnset(table->symbols, name);
    }
    /* Unset in parent if the symbol exists at the higher level */
    else if(table->parent && JE_SymtblGet(table->parent, name)) {
        JE_SymtblUnset(table->parent, name);
    }
}

void JE_SymtblClear(JE_SYMTBL* table, int localonly) {
    JE_MapClear(table->symbols);

    /* clear parent */
    if(!localonly && table->parent) {
        JE_SymtblClear(table->parent, localonly);
    }
}

JE_VAL* JE_SymtblGet(JE_SYMTBL* table, const char* name) {
    JE_VAL* val = NULL;

    if(name && strcmp(name,"LOCAL")==0) {
        if(table) val = JE_SymtblGet(table, NULL);
    }
    else if(name && strcmp(name,"GLOBAL")==0) {
        if(table && table->global) val = JE_SymtblGet(table->global, NULL);
    }
    else if(name && strcmp(name,"UPSCOPE")==0) {
        if(table && table->parent) val = JE_SymtblGet(table->parent, NULL);
    }
    else if(name && strcmp(name,"BUILTIN")==0) {
        if(table && table->builtin) val = JE_SymtblGet(table->builtin, NULL);
    }
    else if(name) {
        if(table && !val) val = JE_MapGet(table->symbols, name);
        if(table && !val) val = JE_SymtblGet(table->parent, name);
    }
    else {
        val = table->symval;
    }

    return val;
}
