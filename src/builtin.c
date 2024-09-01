#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtin.h"
#include "error.h"
#include "eval.h"
#include "func.h"
#include "map.h"
#include "val.h"
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

static VAL* FUNCTION(VEC* args, SYMTBL* table) {
    VAL* spec = args->item[0];
    char* sig = valstr(spec);
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

        return funcval(newcustfunc(args->item[1]->value.n, name, sig, table));
    }

    return NULL;
}


static VAL* PRINT(VEC* args, SYMTBL* table) {
    VAL* last = nullval();

    for(size_t i=0; i<args->length; i++) {
        if(last) freeval(last);
        last = dupval(args->item[i]);

        if(i > 0) printf(" ");
        printf("%s", valstr(last));
    }

    printf("\n");

    return last;
}


static VAL* SQRT(VEC* args, SYMTBL* table) {
    VAL* result = nullval();
    VAL* val = args->item[0];

    switch(val->type) {
        case INT_V      : result = dblval(sqrt(val->value.i)); break;
        case FLOAT_V    : result = dblval(sqrt(val->value.f)); break;
        default         : throwLater("Invalid argument to SQRT(): '%c' (%d)", val->type, val->type); break;
    }

    return result;
}


static VAL* LEN(VEC* args, SYMTBL* table) {
    VAL* result = NULL;
    VAL* val = args->item[0];

    switch(val->type) {
        case STRING_V   : result = intval(strlen(valstr(val))); break;
        case ARRAY_V    : result = intval(val->value.v->length); break;
        case OBJECT_V   : result = intval(val->value.m->length); break;
        default         : throwLater("Type has no length: '%c' (%d)", val->type, val->type); break;
    }

    return result;
}


static VAL* FOR(VEC* args, SYMTBL* table) {
    VAL* result = nullval();
    VAL* last = eval(args->item[0]->value.n, table);

    while(1) {
        freeval(last);
        last = eval(args->item[1]->value.n, table);

        if(!valtrue(last)) break;
        result = eval(args->item[3]->value.n, table);

        freeval(last);
        last = eval(args->item[2]->value.n, table);
    }

    freeval(last);

    return result;
}


static VAL* IF(VEC* args, SYMTBL* table) {
    VAL* result = NULL;

    for(size_t i=0; i<(args->length & ~1UL); i+=2) {
        VAL* cond = eval(args->item[i]->value.n, table);

        if(valtrue(cond)) {
            result = eval(args->item[i+1]->value.n, table);
            break;
        }
    }

    if(!result && (args->length % 2)) {
        result = eval(args->item[args->length-1]->value.n, table);
    }

    return result ? result : nullval();
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

MAP* builtin() {
    if(!BUILTINS) {
        BUILTINS = newmap();

        mapset(BUILTINS, "FUNCTION", funcval(newfunc(FUNCTION, "FUNCTION()", "S.")));
        mapset(BUILTINS, "PRINT"   , funcval(newfunc(PRINT   , "PRINT()"   , "*")));
        mapset(BUILTINS, "SQRT"    , funcval(newfunc(SQRT    , "SQRT()"    , "?")));
        mapset(BUILTINS, "LEN"     , funcval(newfunc(LEN     , "LEN()"     , "?")));
        mapset(BUILTINS, "FOR"     , funcval(newfunc(FOR     , "FOR()"     , "....")));
        mapset(BUILTINS, "IF"      , funcval(newfunc(IF      , "IF()"      , ".**")));
    }

    return BUILTINS;
}
