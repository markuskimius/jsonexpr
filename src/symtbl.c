#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "builtin.h"
#include "symtbl.h"
#include "value.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

SYMTBL* newtable(SYMTBL* parent) {
    SYMTBL* table = calloc(1, sizeof(SYMTBL));
    SYMTBL* pp = parent;

    table->symbols = newmap();
    table->parent = parent;
    table->count = 1;

    /* Add UPSCOPE */
    if(parent) {
        mapset(table->symbols, "UPSCOPE", objvalue(dupmap(parent->symbols)));
    }

    /* Add built-in symbols */
    if(!parent) {
        MAP* bi = builtin();

        while((bi = mapnext(bi))) {
            mapset(table->symbols, bi->key, bi->value);
        }
    }

    return duptable(table);
}


SYMTBL* duptable(SYMTBL* table) {
    table->count++;

    if(table->parent) {
        duptable(table->parent);
    }

    return table;
}


void freetable(SYMTBL* table) {
    table->count--;

    /* Free this table if it's no longer used */
    if(table->count == 0) {
        freemap(table->symbols);
        free(table);
    }

    /* Reduce parent counter */
    if(table->parent) {
        freetable(table->parent);
    }
}


void tableset(SYMTBL* table, const char* name, VALUE* value) {
    /* Set at this level if the symbol exists at this level */
    if(mapget(table->symbols, name)) {
        mapset(table->symbols, name, value);
    }
    /* Set in parent if the symbol exists at the higher level */
    else if(table->parent && tableget(table->parent, name)) {
        tableset(table->parent, name, value);
    }
    /* Otherwise set at this level */
    else {
        mapset(table->symbols, name, value);
    }
}


void tableunset(SYMTBL* table, const char* name) {
    /* Unset at this level if the symbol exists at this level */
    if(mapget(table->symbols, name)) {
        mapunset(table->symbols, name);
    }
    /* Unset in parent if the symbol exists at the higher level */
    else if(table->parent && tableget(table->parent, name)) {
        tableunset(table->parent, name);
    }
}


VALUE* tableget(SYMTBL* table, const char* name) {
    VALUE* value = NULL;

    if(table && !value) value = mapget(table->symbols, name);
    if(table && !value) value = tableget(table->parent, name);

    return value;
}
