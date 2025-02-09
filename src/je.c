#include <stdlib.h>
#include "je.h"
#include "je_error.h"
#include "je_map.h"
#include "je_util.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_COMPILED* JE_Compile(const char* code) {
    JE_COMPILED* compiled = JE_Calloc(1, sizeof(JE_COMPILED));

    compiled->ast = JE_Parse(code);
    compiled->symtbl = JE_SymtblNew(NULL);

    return compiled;
}

void JE_FreeCompiled(JE_COMPILED* compiled) {
    JE_NodeDelete(compiled->ast);
    JE_SymtblDelete(compiled->symtbl);

    compiled->ast = NULL;
    compiled->symtbl = NULL;

    free(compiled);
}

JE_VAL* JE_EvalCompiled(JE_COMPILED* compiled) {
    return JE_EvalNode(compiled->ast, compiled->symtbl);
}

int JE_SetSymbols(JE_COMPILED* compiled, const char* json) {
    JE_COMPILED* c = JE_Compile(json);
    JE_VAL* r = JE_EvalCompiled(c);
    int isok = 0;

    if(r->type == JE_OBJECT_V) {
        JE_MAP* m = r->value.m;

        while((m = JE_MapNext(m))) {
            const char* key = JE_MapKey(m);
            JE_VAL* value = JE_MapVal(m);

            JE_SymtblSet(compiled->symtbl, key, JE_ValDup(value), 1);
        }

        isok = 1;
    }

    JE_ValDelete(r);
    JE_FreeCompiled(c);

    return isok;
}

void JE_ClearSymbols(JE_COMPILED* compiled) {
    JE_SymtblClear(compiled->symtbl, 1);
}
