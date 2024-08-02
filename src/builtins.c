#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtins.h"
#include "ast.h"
#include "map.h"
#include "eval.h"
#include "func.h"
#include "ufunc.h"
#include "value.h"


/* ***************************************************************************
* TYPES
*/

typedef struct USERFN {
    AST_NODE* body;
} USERFN;


/* ***************************************************************************
* GLOBALS
*/

static MAP* BUILTINS = NULL;
static size_t nufunc = 0;


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static VALUE* FUNCTION(size_t argc, AST_NODE** argv, SYM_TABLE* table) {
    char name[64];

    snprintf(name, sizeof(name), "FUNCTION#%zd()", ++nufunc);

    return newuserfunc(newufunc(name, argv[1], 0, MAXARGS));
}


static VALUE* PRINT(size_t argc, AST_NODE** argv, SYM_TABLE* table) {
    VALUE* last = newnull();

    for(size_t i=0; i<argc; i++) {
        AST_NODE* node = argv[i];

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

MAP* builtins() {
    if(!BUILTINS) {
        BUILTINS = newmap();

        setmap(BUILTINS, "FUNCTION", newbuiltin(newfunc("FUNCTION()", FUNCTION, 2,       2)));
        setmap(BUILTINS, "PRINT"   , newbuiltin(newfunc("PRINT()"   , PRINT   , 0, MAXARGS)));
    }

    return BUILTINS;
}
