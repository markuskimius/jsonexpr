#include <stdlib.h>
#include "je.h"
#include "je_error.h"
#include "je_map.h"
#include "je_util.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_COMPILED* je_compile(const char* code) {
    JE_COMPILED* compiled = JE_Calloc(1, sizeof(JE_COMPILED));

    compiled->ast = je_parse(code);
    compiled->symtbl = je_newtable(NULL);

    return compiled;
}

void je_free(JE_COMPILED* compiled) {
    je_freenode(compiled->ast);
    je_freetable(compiled->symtbl);

    compiled->ast = NULL;
    compiled->symtbl = NULL;

    JE_Free(compiled);
}

JE_VAL* je_evalc(JE_COMPILED* compiled) {
    return je_eval(compiled->ast, compiled->symtbl);
}

int je_setSymbols(JE_COMPILED* compiled, const char* json) {
    JE_COMPILED* c = je_compile(json);
    JE_VAL* r = je_evalc(c);
    int isok = 0;

    if(r->type == JE_OBJECT_V) {
        JE_MAP* m = r->value.m;

        while((m = je_mapnext(m))) {
            const char* key = je_mapkey(m);
            JE_VAL* value = je_mapval(m);

            je_tableset(compiled->symtbl, key, je_dupval(value), 1);
        }

        isok = 1;
    }

    je_freeval(r);
    je_free(c);

    return isok;
}

void je_clearSymbols(JE_COMPILED* compiled) {
    je_tableclear(compiled->symtbl, 1);
}
