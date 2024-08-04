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
    VALUE* spec = args->item[0];
    char* sig = strvalue(spec);
    int isok = 1;

    if(spec->type == STRING_V) {
        int nstars = 0;

        /* Validate the signature argument */
        for(size_t i=0; i<strlen(sig); i++) {
            /* Validate signature */
            if(!strchr("BIDSAOF?.*", sig[i])) {
                fprintf(stderr, "%s: Invalid function argument signature: %c\n", __FUNCTION__, sig[i]);
                isok = 0;
                break;
            }

            /* Signature can only end with a star */
            if(nstars && sig[i] != '*') {
                fprintf(stderr, "%s: '*' must terminate function argument signature\n", __FUNCTION__);
                isok = 0;
                break;
            }

            /* Maximum # of stars is 2 */
            if(sig[i] == '*' && nstars == 2) {
                fprintf(stderr, "%s: Too many '*', maximum is 2\n", __FUNCTION__);
                isok = 0;
                break;
            }

            if(sig[i] == '*') nstars++;
        }
    }
    else {
        fprintf(stderr, "%s: Invalid argument type, expected string but got %c\n", __FUNCTION__, spec->type);
        isok = 0;
    }

    if(isok) {
        char name[64];

        snprintf(name, sizeof(name), "FUNCTION#%zd()", ++nufunc);

        return newuserfunc(newufunc(args->item[1]->value.node, name, sig));
    }

    return newnull();
}


static VALUE* PRINT(VEC* args, SYM_TABLE* table) {
    VALUE* last = newnull();

    for(size_t i=0; i<args->length; i++) {
        if(last) freevalue(last);
        last = dupvalue(args->item[i]);

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

        setmap(BUILTINS, "FUNCTION", newbuiltin(newfunc(FUNCTION, "FUNCTION()", "S.")));
        setmap(BUILTINS, "PRINT"   , newbuiltin(newfunc(PRINT   , "PRINT()"   , "*")));
    }

    return BUILTINS;
}
