#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "builtin.h"
#include "symtable.h"
#include "value.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

SYM_TABLE* newtable(SYM_TABLE* parent) {
    SYM_TABLE* table = calloc(1, sizeof(SYM_TABLE));
    SYM_TABLE* pp = parent;

    table->symbols = newmap();
    table->parent = parent;
    table->count = 1;

    /* Add UPSCOPE */
    if(parent) {
        setmap(table->symbols, "UPSCOPE", objvalue(dupmap(parent->symbols)));
    }

    /* Add built-in symbols */
    if(!parent) {
        MAP* bi = builtin();

        while((bi = nextmap(bi))) {
            setmap(table->symbols, bi->key, bi->value);
        }
    }

    return duptable(table);
}


SYM_TABLE* duptable(SYM_TABLE* table) {
    table->count++;

    if(table->parent) {
        duptable(table->parent);
    }

    return table;
}


void freetable(SYM_TABLE* table) {
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


void settable(SYM_TABLE* table, const char* name, VALUE* value) {
    /* Set at this level if the symbol exists at this level */
    if(getmap(table->symbols, name)) {
        setmap(table->symbols, name, value);
    }
    /* Set in parent if the symbol exists at the higher level */
    else if(table->parent && gettable(table->parent, name)) {
        settable(table->parent, name, value);
    }
    /* Otherwise set at this level */
    else {
        setmap(table->symbols, name, value);
    }
}


void unsettable(SYM_TABLE* table, const char* name) {
    /* Unset at this level if the symbol exists at this level */
    if(getmap(table->symbols, name)) {
        unsetmap(table->symbols, name);
    }
    /* Unset in parent if the symbol exists at the higher level */
    else if(table->parent && gettable(table->parent, name)) {
        unsettable(table->parent, name);
    }
}


VALUE* gettable(SYM_TABLE* table, const char* name) {
    VALUE* value = NULL;

    if(table && !value) value = getmap(table->symbols, name);
    if(table && !value) value = gettable(table->parent, name);

    return value;
}
