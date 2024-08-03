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

static VALUE* FUNCTION(size_t argc, NODE** argv, SYM_TABLE* table) {
    char name[64];
    char* sig = NULL;
    VALUE* spec = eval(argv[0], table);
    VALUE* result = newnull();

    snprintf(name, sizeof(name), "FUNCTION#%zd()", ++nufunc);

    switch(spec->type) {
        case STRING_V:
            break;

        default:
            fprintf(stderr, "Invalid argument type, expected string but got %c\n", spec->type);
    }

    freevalue(spec);

    return newuserfunc(newufunc(argv[1], name, sig));
}


static VALUE* PRINT(size_t argc, NODE** argv, SYM_TABLE* table) {
    VALUE* last = newnull();

    for(size_t i=0; i<argc; i++) {
        NODE* node = argv[i];

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
