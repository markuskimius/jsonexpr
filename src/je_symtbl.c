#include <stdio.h>
#include <stdlib.h>
#include "je_builtin.h"
#include "je_map.h"
#include "je_symtbl.h"
#include "je_val.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_SYMTBL* je_newtable(JE_SYMTBL* parent) {
    JE_SYMTBL* table = calloc(1, sizeof(JE_SYMTBL));
    JE_SYMTBL* pp = parent;

    table->symbols = je_newmap();
    table->parent = parent;
    table->count = 1;

    /* Add UPSCOPE */
    if(table->parent) {
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
        je_freemap(table->symbols);
        free(table);
    }
}


void je_tableset(JE_SYMTBL* table, const char* name, JE_VAL* val) {
    /* Set at this level if the symbol exists at this level */
    if(je_mapget(table->symbols, name)) {
        je_mapset(table->symbols, name, val);
    }
    /* Set in parent if the symbol exists at the higher level */
    else if(table->parent && je_tableget(table->parent, name)) {
        je_tableset(table->parent, name, val);
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


JE_VAL* je_tableget(JE_SYMTBL* table, const char* name) {
    JE_VAL* val = NULL;

    if(table && !val) val = je_mapget(table->symbols, name);
    if(table && !val) val = je_tableget(table->parent, name);

    return val;
}
