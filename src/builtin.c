#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtin.h"
#include "eval.h"
#include "func.h"
#include "map.h"
#include "node.h"
#include "ufunc.h"
#include "value.h"
#include "vector.h"


/* ***************************************************************************
* TYPES
*/

typedef struct USERFN {
    NODE* body;
} USERFN;


/* ***************************************************************************
* GLOBALS
*/

static MAP* BUILTINS = NULL;
static size_t nufunc = 0;


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static VALUE* FUNCTION(VEC* args, SYM_TABLE* table) {
    char name[64];
    char* sig = NULL;
    VALUE* spec = eval(args->item[0]->value.node, table);
    VALUE* result = newnull();

    snprintf(name, sizeof(name), "FUNCTION#%zd()", ++nufunc);

    switch(spec->type) {
        case STRING_V:
            break;

        default:
            fprintf(stderr, "Invalid argument type, expected string but got %c\n", spec->type);
    }

    freevalue(spec);

    return newuserfunc(newufunc(args->item[1]->value.node, name, sig));
}


static VALUE* PRINT(VEC* args, SYM_TABLE* table) {
    VALUE* last = newnull();

    for(size_t i=0; i<args->length; i++) {
        NODE* node = args->item[i]->value.node;

        if(last) freevalue(last);
        last = eval(node, table);

        if(i > 0) printf(" ");
        printf("%s", strvalue(last));
    }

    printf("\n");

    return last;
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

MAP* builtin() {
    if(!BUILTINS) {
        BUILTINS = newmap();

        setmap(BUILTINS, "FUNCTION", newbuiltin(newfunc(FUNCTION, "FUNCTION()", "SN")));
        setmap(BUILTINS, "PRINT"   , newbuiltin(newfunc(PRINT   , "PRINT()"   , "*")));
    }

    return BUILTINS;
}
