#include <stdio.h>
#include <stdlib.h>
#include "symtable.h"
#include "builtins.h"
#include "value.h"
#include "map.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

SYM_TABLE* newtable(SYM_TABLE* parent) {
    SYM_TABLE* table = calloc(1, sizeof(SYM_TABLE));

    table->symbols = newmap();
    table->parent = parent;

    /* Add built-in symbols */
    if(!parent) {
        MAP* bi = builtins();

        while((bi = nextmap(bi))) {
            setmap(table->symbols, bi->key, bi->value);
        }
    }

    return table;
}


VALUE* gettable(SYM_TABLE* table, const char* name) {
    VALUE* value = NULL;

    if(table && !value) value = getmap(table->symbols, name);
    if(table && !value) value = gettable(table->parent, name);

    return value;
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
