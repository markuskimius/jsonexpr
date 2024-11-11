#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "je_builtin.h"
#include "je_map.h"
#include "je_symtbl.h"
#include "je_val.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_SYMTBL* je_newtable(JE_SYMTBL* parent) {
    JE_SYMTBL* table = calloc(1, sizeof(JE_SYMTBL));
    JE_SYMTBL* global = parent ? parent->global : table;

    table->symbols = je_newmap();
    table->symval = je_objval(je_dupmap(table->symbols));
    table->parent = parent;
    table->global = global;
    table->count = 1;

    /* Add UPSCOPE */
    if(parent) {
        je_duptable(table->parent);
        je_mapset(table->symbols, "UPSCOPE", je_objval(je_dupmap(parent->symbols)));
    }

    /* Add built-in symbols */
    if(!parent) {
        JE_MAP* bi = je_binfns();

        while((bi = je_mapnext(bi))) {
            je_mapset(table->symbols, bi->key, bi->value);
        }
    }

    return je_duptable(table);
}


JE_SYMTBL* je_duptable(JE_SYMTBL* table) {
    table->count++;

    if(table->parent) {
        je_duptable(table->parent);
    }

    return table;
}


void je_freetable(JE_SYMTBL* table) {
    table->count--;

    /* Reduce parent counter */
    if(table->parent) {
        je_freetable(table->parent);
    }

    /* Free this table if it's no longer used */
    if(table->count == 0) {
        je_freeval(table->symval);
        je_freemap(table->symbols);
        free(table);
    }
}


void je_tableset(JE_SYMTBL* table, const char* name, JE_VAL* val, int localonly) {
    /* Set at this level if the symbol exists at this level */
    if(je_mapget(table->symbols, name)) {
        je_mapset(table->symbols, name, val);
    }
    /* Set in parent if the symbol exists at the higher level */
    else if(!localonly && table->parent && je_tableget(table->parent, name)) {
        je_tableset(table->parent, name, val, localonly);
    }
    /* Otherwise set at this level */
    else {
        je_mapset(table->symbols, name, val);
    }
}


void je_tableunset(JE_SYMTBL* table, const char* name) {
    /* Unset at this level if the symbol exists at this level */
    if(je_mapget(table->symbols, name)) {
        je_mapunset(table->symbols, name);
    }
    /* Unset in parent if the symbol exists at the higher level */
    else if(table->parent && je_tableget(table->parent, name)) {
        je_tableunset(table->parent, name);
    }
}


void je_tableclear(JE_SYMTBL* table, int localonly) {
    je_mapclear(table->symbols);

    /* clear parent */
    if(!localonly && table->parent) {
        je_tableclear(table->parent, localonly);
    }
}


JE_VAL* je_tableget(JE_SYMTBL* table, const char* name) {
    JE_VAL* val = NULL;

    if(name && strcmp(name,"LOCAL")==0) {
        if(table) val = je_tableget(table, NULL);
    }
    else if(name && strcmp(name,"GLOBAL")==0) {
        if(table && table->global) val = je_tableget(table->global, NULL);
    }
    else if(name && strcmp(name,"UPSCOPE")==0) {
        if(table && table->parent) val = je_tableget(table->parent, NULL);
    }
    else if(name) {
        if(table && !val) val = je_mapget(table->symbols, name);
        if(table && !val) val = je_tableget(table->parent, name);
    }
    else {
        val = table->symval;
    }

    return val;
}
