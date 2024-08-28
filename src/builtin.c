#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtin.h"
#include "eval.h"
#include "func.h"
#include "map.h"
#include "node.h"
#include "throw.h"
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
static size_t ncustfunc = 0;


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static VALUE* FUNCTION(VEC* args, SYM_TABLE* table) {
    VALUE* spec = args->item[0];
    char* sig = strdecoded(spec);
    int isok = 1;

    if(spec->type == STRING_V) {
        int nstars = 0;

        /* Validate the signature argument */
        for(size_t i=0; i<strlen(sig); i++) {
            /* Validate signature */
            if(!strchr("BIDSAOF?.*", sig[i])) {
                raise("Invalid function argument signature: %c", sig[i]);
                isok = 0;
                break;
            }

            /* Signature can only end with a star */
            if(nstars && sig[i] != '*') {
                raise("'*' must terminate function argument signature");
                isok = 0;
                break;
            }

            /* Maximum # of stars is 2 */
            if(sig[i] == '*' && nstars == 2) {
                raise("Too many '*', maximum is 2");
                isok = 0;
                break;
            }

            if(sig[i] == '*') nstars++;
        }
    }
    else {
        raise("Invalid argument type, expected string but got %c", spec->type);
        isok = 0;
    }

    if(isok) {
        char name[64];

        snprintf(name, sizeof(name), "FUNCTION#%zd()", ++ncustfunc);

        return funcvalue(newcustfunc(args->item[1]->value.n, name, sig, table));
    }

    return NULL;
}


static VALUE* PRINT(VEC* args, SYM_TABLE* table) {
    VALUE* last = nullvalue();

    for(size_t i=0; i<args->length; i++) {
        if(last) freevalue(last);
        last = dupvalue(args->item[i]);

        if(i > 0) printf(" ");
        printf("%s", strdecoded(last));
    }

    printf("\n");

    return last;
}


static VALUE* SQRT(VEC* args, SYM_TABLE* table) {
    VALUE* result = nullvalue();
    VALUE* value = args->item[0];

    switch(value->type) {
        case INT_V      : result = dblvalue(sqrt(value->value.i)); break;
        case FLOAT_V    : result = dblvalue(sqrt(value->value.f)); break;
        default         : raise("Invalid argument to SQRT(): '%c' (%d)", value->type, value->type); break;
    }

    return result;
}


static VALUE* LEN(VEC* args, SYM_TABLE* table) {
    VALUE* result = NULL;
    VALUE* value = args->item[0];

    switch(value->type) {
        case STRING_V   : result = intvalue(strlen(strdecoded(value))); break;
        case ARRAY_V    : result = intvalue(value->value.v->length); break;
        case OBJECT_V   : result = intvalue(value->value.m->length); break;
        default         : raise("Type has no length: '%c' (%d)", value->type, value->type); break;
    }

    return result;
}


static VALUE* FOR(VEC* args, SYM_TABLE* table) {
    VALUE* result = nullvalue();

    for(eval(args->item[0]->value.n, table); istrue(eval(args->item[1]->value.n, table)); eval(args->item[2]->value.n, table)) {
        result = eval(args->item[3]->value.n, table);
    }

    return result;
}


static VALUE* IF(VEC* args, SYM_TABLE* table) {
    VALUE* result = NULL;

    for(size_t i=0; i<(args->length & ~1UL); i+=2) {
        VALUE* cond = eval(args->item[i]->value.n, table);

        if(istrue(cond)) {
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

        setmap(BUILTINS, "FUNCTION", funcvalue(newfunc(FUNCTION, "FUNCTION()", "S.")));
        setmap(BUILTINS, "PRINT"   , funcvalue(newfunc(PRINT   , "PRINT()"   , "*")));
        setmap(BUILTINS, "SQRT"    , funcvalue(newfunc(SQRT    , "SQRT()"    , "?")));
        setmap(BUILTINS, "LEN"     , funcvalue(newfunc(LEN     , "LEN()"     , "?")));
        setmap(BUILTINS, "FOR"     , funcvalue(newfunc(FOR     , "FOR()"     , "?...")));
        setmap(BUILTINS, "IF"      , funcvalue(newfunc(IF      , "IF()"      , ".**")));
    }

    return BUILTINS;
}
