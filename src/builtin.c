#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtin.h"
#include "eval.h"
#include "func.h"
#include "map.h"
#include "node.h"
#include "error.h"
#include "value.h"
#include "vec.h"


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
static size_t ncustfunc = 0;


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static VALUE* FUNCTION(VEC* args, SYMTBL* table) {
    VALUE* spec = args->item[0];
    char* sig = valuestr(spec);
    int isok = 1;

    if(spec->type == STRING_V) {
        int nstars = 0;

        /* Validate the signature argument */
        for(size_t i=0; i<strlen(sig); i++) {
            /* Validate signature */
            if(!strchr("BIDSAOF?.*", sig[i])) {
                throwLater("Invalid function argument signature: %c", sig[i]);
                isok = 0;
                break;
            }

            /* Signature can only end with a star */
            if(nstars && sig[i] != '*') {
                throwLater("'*' must terminate function argument signature");
                isok = 0;
                break;
            }

            /* Maximum # of stars is 2 */
            if(sig[i] == '*' && nstars == 2) {
                throwLater("Too many '*', maximum is 2");
                isok = 0;
                break;
            }

            if(sig[i] == '*') nstars++;
        }
    }
    else {
        throwLater("Invalid argument type, expected string but got %c", spec->type);
        isok = 0;
    }

    if(isok) {
        char name[64];

        snprintf(name, sizeof(name), "FUNCTION#%zd()", ++ncustfunc);

        return funcvalue(newcustfunc(args->item[1]->value.n, name, sig, table));
    }

    return NULL;
}


static VALUE* PRINT(VEC* args, SYMTBL* table) {
    VALUE* last = nullvalue();

    for(size_t i=0; i<args->length; i++) {
        if(last) freevalue(last);
        last = dupvalue(args->item[i]);

        if(i > 0) printf(" ");
        printf("%s", valuestr(last));
    }

    printf("\n");

    return last;
}


static VALUE* SQRT(VEC* args, SYMTBL* table) {
    VALUE* result = nullvalue();
    VALUE* value = args->item[0];

    switch(value->type) {
        case INT_V      : result = dblvalue(sqrt(value->value.i)); break;
        case FLOAT_V    : result = dblvalue(sqrt(value->value.f)); break;
        default         : throwLater("Invalid argument to SQRT(): '%c' (%d)", value->type, value->type); break;
    }

    return result;
}


static VALUE* LEN(VEC* args, SYMTBL* table) {
    VALUE* result = NULL;
    VALUE* value = args->item[0];

    switch(value->type) {
        case STRING_V   : result = intvalue(strlen(valuestr(value))); break;
        case ARRAY_V    : result = intvalue(value->value.v->length); break;
        case OBJECT_V   : result = intvalue(value->value.m->length); break;
        default         : throwLater("Type has no length: '%c' (%d)", value->type, value->type); break;
    }

    return result;
}


static VALUE* FOR(VEC* args, SYMTBL* table) {
    VALUE* result = nullvalue();
    VALUE* last = eval(args->item[0]->value.n, table);

    while(1) {
        freevalue(last);
        last = eval(args->item[1]->value.n, table);

        if(!valuetrue(last)) break;
        result = eval(args->item[3]->value.n, table);

        freevalue(last);
        last = eval(args->item[2]->value.n, table);
    }

    freevalue(last);

    return result;
}


static VALUE* IF(VEC* args, SYMTBL* table) {
    VALUE* result = NULL;

    for(size_t i=0; i<(args->length & ~1UL); i+=2) {
        VALUE* cond = eval(args->item[i]->value.n, table);

        if(valuetrue(cond)) {
            result = eval(args->item[i+1]->value.n, table);
            break;
        }
    }

    if(!result && (args->length % 2)) {
        result = eval(args->item[args->length-1]->value.n, table);
    }

    return result ? result : nullvalue();
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

MAP* builtin() {
    if(!BUILTINS) {
        BUILTINS = newmap();

        mapset(BUILTINS, "FUNCTION", funcvalue(newfunc(FUNCTION, "FUNCTION()", "S.")));
        mapset(BUILTINS, "PRINT"   , funcvalue(newfunc(PRINT   , "PRINT()"   , "*")));
        mapset(BUILTINS, "SQRT"    , funcvalue(newfunc(SQRT    , "SQRT()"    , "?")));
        mapset(BUILTINS, "LEN"     , funcvalue(newfunc(LEN     , "LEN()"     , "?")));
        mapset(BUILTINS, "FOR"     , funcvalue(newfunc(FOR     , "FOR()"     , "....")));
        mapset(BUILTINS, "IF"      , funcvalue(newfunc(IF      , "IF()"      , ".**")));
    }

    return BUILTINS;
}
