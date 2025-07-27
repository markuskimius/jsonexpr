#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "je_ast.h"
#include "je_const.h"
#include "je_error.h"
#include "je_map.h"
#include "je_node.h"
#include "je_ntype.h"
#include "je_parse.h"
#include "je_ref.h"
#include "je_symbols.h"
#include "je_symmap.h"
#include "je_type.h"
#include "je_util.h"
#include "je_value.h"
#include "je_vec.h"


/* ***************************************************************************
* PRIVATE VARIABLES
*/

static size_t _nfunc = 0;


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static JE_VALUE _USERFUNC(JE_VEC* args, const JE_NODE* code, JE_MAP* symmap, const JE_ASTINFO* info) {
    JE_VALUE result = JE_ValueNul();
    JE_MAP* subscope = JE_SymmapCreateScope(symmap);

    JE_SymmapSet(subscope, JE_StrCreate(JE_Strdup("ARGS")), JE_ValueRefVec(JE_RefVecDup(args)), 1);

    result = JE_NCALL(code,eval,subscope,info,NULL);

    JE_SymmapDestroy(subscope);

    return result;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - BUILT-IN FUNCTIONS
*/

static JE_VALUE _BINARY(JE_VEC* args, const JE_NODE* code, JE_MAP* symmap, const JE_ASTINFO* info) {
    int64_t i64 = args->data[0].data.i64;
    char output[sizeof(i64)*8+1];

    /* Digits */
    for(int i=0; i<sizeof(i64)*8; i++) {
        output[i] = i64<0 ? '1' : '0';
        i64 <<= 1;
    }

    /* NUL terminate */
    output[sizeof(i64)*8] = '\0';

    return JE_ValueStr(JE_StrCreate(JE_Strdup(output)));
}

static JE_VALUE _CEIL(JE_VEC* args, const JE_NODE* code, JE_MAP* symmap, const JE_ASTINFO* info) {
    const JE_VALUE value = args->data[0];
    JE_VALUE f64 = JE_VCALL(&value,toF64);

    return JE_ValueI64(ceil(f64.data.f64));
}

static JE_VALUE _EVAL(JE_VEC* args, const JE_NODE* code, JE_MAP* symmap, const JE_ASTINFO* info) {
    const JE_VALUE value = args->data[0];
    JE_VALUE result = JE_ValueNul();

    switch(JE_VCALL(&value,getType)) {
        case JE_STR_T : {
            JE_AST ast = JE_Parse((char*) value.data.str.data);

            if(ast.isok) result = JE_AstEval(ast, symmap, 0);

            JE_AstDestroy(&ast);
            break;
        }

        case JE_NOD_T : {
            result = JE_NCALL(value.data.nod.node,eval,symmap,info,NULL);
            break;
        }

        default : {
            result = JE_VCALL(&value,clone);
            break;
        }
    }

    return result;
}

static JE_VALUE _FLOOR(JE_VEC* args, const JE_NODE* code, JE_MAP* symmap, const JE_ASTINFO* info) {
    const JE_VALUE value = args->data[0];
    JE_VALUE f64 = JE_VCALL(&value,toF64);

    return JE_ValueI64(floor(f64.data.f64));
}

static JE_VALUE _FOR(JE_VEC* args, const JE_NODE* code, JE_MAP* symmap, const JE_ASTINFO* info) {
    const JE_NODE* init = args->data[0].data.nod.node;
    const JE_NODE* cond = args->data[1].data.nod.node;
    const JE_NODE* iter = args->data[2].data.nod.node;
    const JE_NODE* stmt = args->data[3].data.nod.node;
    JE_VALUE initlast = JE_ValueNul();
    JE_VALUE condlast = JE_ValueNul();
    JE_VALUE iterlast = JE_ValueNul();
    JE_VALUE stmtlast = JE_ValueNul();

    assert(args->length == 4);
    assert(args->data[0].type == &JE_typeNod);
    assert(args->data[1].type == &JE_typeNod);
    assert(args->data[2].type == &JE_typeNod);
    assert(args->data[3].type == &JE_typeNod);

    /* Initialize */
    initlast = JE_NCALL(init,eval,symmap,info,NULL);

    while(1) {
        /* Evaluate the condition */
        JE_VCALL(&condlast,destroy);
        condlast = JE_NCALL(cond,eval,symmap,info,NULL);

        /* Exit if the condition is false */
        if(!JE_VCALL(&condlast,toTru)) break;

        /* Evaluate the statement */
        JE_VCALL(&stmtlast,destroy);
        stmtlast = JE_NCALL(stmt,eval,symmap,info,NULL);

        /* Evaluate the iteration */
        JE_VCALL(&iterlast,destroy);
        iterlast = JE_NCALL(iter,eval,symmap,info,NULL);
    }

    /* Destroy the last evaluated except statement */
    JE_VCALL(&initlast,destroy);
    JE_VCALL(&condlast,destroy);
    JE_VCALL(&iterlast,destroy);

    /* Return the last evaluated statement */
    return stmtlast;
}

static JE_VALUE _FOREACH(JE_VEC* args, const JE_NODE* code, JE_MAP* symmap, const JE_ASTINFO* info) {
    const JE_NODE* name = args->data[0].data.nod.node;
    const JE_VALUE iter = args->data[1];
    const JE_NODE* stmt = args->data[2].data.nod.node;
    JE_VALUE result = JE_ValueNul();

    /* Validate */
    if(name->ntype != &JE_ntypeIdent) {
        JE_RuntimeError(JE_NCALL(name,tokenBegin,info), JE_NCALL(name,tokenEnd,info), "Identifier expected, got %s", JE_NCALL(name,getTypeCstr));

        return result;
    }

    switch(JE_VCALL(&iter,getType)) {
        case JE_VEC_T: {
            JE_VEC* vec = iter.data.vec;

            for(size_t i=0; i<vec->length; i++) {
                JE_STR key = JE_StrClone(name->data.id);
                JE_VALUE item = JE_VCALL(&vec->data[i],clone);

                JE_SymmapSet(symmap, key, item, 0);             /* Next iterator */

                JE_VCALL(&result,destroy);                      /* Destroy the previous result */
                result = JE_NCALL(stmt,eval,symmap,info,NULL);  /* Evaluate the next result */
            }

            break;
        }

        case JE_MAP_T: {
            JE_MAP* map = iter.data.map;

            for(size_t i=0; i<map->size; i++) {
                JE_STR key = JE_StrClone(name->data.id);
                JE_VEC pair = JE_VecCreate(2);
                JE_VALUE first = JE_ValueStr(JE_StrClone(map->key[i]));
                JE_VALUE second = JE_VCALL(&map->value[i],clone);

                JE_VecPush(&pair, first);
                JE_VecPush(&pair, second);

                JE_SymmapSet(symmap, key, JE_ValueVec(pair), 0); /* Next iterator */

                JE_VCALL(&result,destroy);                       /* Destroy the previous result */
                result = JE_NCALL(stmt,eval,symmap,info,NULL);   /* Evaluate the next result */
            }

            break;
        }

        default:
            JE_RuntimeError(JE_NCALL(name,tokenBegin,info), JE_NCALL(name,tokenEnd,info), "ARRAY or OBJECT expected, got %s", JE_VCALL(&iter,getTypeCstr));
    }

    /* Return the last evaluated statement */
    return result;
}

static JE_VALUE _FUNCTION(JE_VEC* args, const JE_NODE* code, JE_MAP* symmap, const JE_ASTINFO* info) {
    const JE_NODE* nspec = args->data[0].data.nod.node;
    const JE_NODE* body = args->data[1].data.nod.node;
    JE_VALUE spec = JE_NCALL(nspec,eval,symmap,info,NULL);
    int isok = 1;

    /* Validate spec */
    if(spec.type == &JE_typeStr) {
        const JE_STR argfmt = spec.data.str;

        for(size_t i=0; i<argfmt.length; i++) {
            int c = argfmt.data[i];

            switch(c) {
                case JE_NUL_T : break;
                case JE_TRU_T : break;
                case JE_I64_T : break;
                case JE_F64_T : break;
                case JE_STR_T : break;
                case JE_MAP_T : break;
                case JE_VEC_T : break;
                case JE_FUN_T : break;
                case JE_NOD_T : break;
                case '#'      : break;
                case '*'      : break;
                case '.'      : break;
                case '?'      : break;
                default       : {
                    JE_RuntimeError(JE_NCALL(nspec,tokenBegin,info), JE_NCALL(nspec,tokenEnd,info), "Invalid function argument specifier '%c' at position %zu as the first argument to FUNCTION()", c, i);
                    isok = 0;
                }
            }
        }
    }
    else {
        JE_RuntimeError(JE_NCALL(nspec,tokenBegin,info), JE_NCALL(nspec,tokenEnd,info), "The first argument to FUNCTION() must be a STRING, got %s", JE_VCALL(&spec,getTypeCstr));
        isok = 0;
    }

    if(isok) {
        char* name = NULL;

        asprintf(&name, "FUNCTION#%zu", _nfunc++);

        return JE_ValueFun(JE_FunCreate(name, _USERFUNC, (char*)spec.data.str.data, body, symmap));
    }

    JE_VCALL(&spec,destroy);

    return JE_ValueNul();
}

static JE_VALUE _HEX(JE_VEC* args, const JE_NODE* code, JE_MAP* symmap, const JE_ASTINFO* info) {
    uint64_t u64 = args->data[0].data.i64;
    char output[sizeof(u64)*2+1];

    /* Digits */
    for(int i=0; i<sizeof(u64)*2; i++) {
        int j = sizeof(u64)*2-i-1;

        switch(u64 & 0xf) {
            case 0x0 : output[j] = '0'; break;
            case 0x1 : output[j] = '1'; break;
            case 0x2 : output[j] = '2'; break;
            case 0x3 : output[j] = '3'; break;
            case 0x4 : output[j] = '4'; break;
            case 0x5 : output[j] = '5'; break;
            case 0x6 : output[j] = '6'; break;
            case 0x7 : output[j] = '7'; break;
            case 0x8 : output[j] = '8'; break;
            case 0x9 : output[j] = '9'; break;
            case 0xA : output[j] = 'A'; break;
            case 0xB : output[j] = 'B'; break;
            case 0xC : output[j] = 'C'; break;
            case 0xD : output[j] = 'D'; break;
            case 0xE : output[j] = 'E'; break;
            case 0xF : output[j] = 'F'; break;
        }

        u64 >>= 4;
    }

    /* NUL terminate */
    output[sizeof(u64)*2] = '\0';

    return JE_ValueStr(JE_StrCreate(JE_Strdup(output)));
}

static JE_VALUE _IF(JE_VEC* args, const JE_NODE* code, JE_MAP* symmap, const JE_ASTINFO* info) {
    JE_VALUE result = JE_ValueNul();
    size_t i = 0;

    /* Cond,IfTrue */
    for(i=0; i+1<args->length; i+=2) {
        JE_VALUE cond = JE_NCALL(args->data[i].data.nod.node,eval,symmap,info,NULL);
        int istru = JE_VCALL(&cond,toTru);

        JE_VCALL(&cond,destroy);

        if(istru) {
            result = JE_NCALL(args->data[i+1].data.nod.node,eval,symmap,info,NULL);
            break;
        }
    }

    /* Else */
    if(i == args->length-1) {
        result = JE_NCALL(args->data[i].data.nod.node,eval,symmap,info,NULL);
    }

    return result;
}

static JE_VALUE _LEN(JE_VEC* args, const JE_NODE* code, JE_MAP* symmap, const JE_ASTINFO* info) {
    const JE_VALUE value = args->data[0];
    JE_VALUE result = JE_VCALL(&value,length);

    return result;
}

static JE_VALUE _PRINT(JE_VEC* args, const JE_NODE* code, JE_MAP* symmap, const JE_ASTINFO* info) {
    int64_t i = 0;

    for(i=0; i<args->length; i++) {
        char* cstr = JE_VCALL(&args->data[i],toCstr);

        if(i) printf(" ");
        printf("%s", cstr);

        JE_Free(cstr);
    }

    printf("\n");
    fflush(stdout);

    return JE_ValueI64(i);
}

static JE_VALUE _ROUND(JE_VEC* args, const JE_NODE* code, JE_MAP* symmap, const JE_ASTINFO* info) {
    const JE_VALUE value = args->data[0];
    JE_VALUE f64 = JE_VCALL(&value,toF64);

    return JE_ValueI64(round(f64.data.f64));
}

static JE_VALUE _SQRT(JE_VEC* args, const JE_NODE* code, JE_MAP* symmap, const JE_ASTINFO* info) {
    const JE_VALUE value = args->data[0];
    JE_VALUE f64 = JE_VCALL(&value,toF64);

    return JE_ValueF64(sqrt(f64.data.f64));
}

static JE_VALUE _STRLEN(JE_VEC* args, const JE_NODE* code, JE_MAP* symmap, const JE_ASTINFO* info) {
    const JE_VALUE value = args->data[0];

    return JE_ValueI64(JE_StrLength(value.data.str));
}

static JE_VALUE _WHILE(JE_VEC* args, const JE_NODE* code, JE_MAP* symmap, const JE_ASTINFO* info) {
    const JE_NODE* cond = args->data[0].data.nod.node;
    const JE_NODE* stmt = args->data[1].data.nod.node;
    JE_VALUE condlast = JE_ValueNul();
    JE_VALUE stmtlast = JE_ValueNul();

    assert(args->length == 2);
    assert(args->data[0].type == &JE_typeNod);
    assert(args->data[1].type == &JE_typeNod);

    while(1) {
        /* Evaluate the condition */
        JE_VCALL(&condlast,destroy);
        condlast = JE_NCALL(cond,eval,symmap,info,NULL);

        /* Exit if the condition is false */
        if(!JE_VCALL(&condlast,toTru)) break;

        /* Evaluate the statement */
        JE_VCALL(&stmtlast,destroy);
        stmtlast = JE_NCALL(stmt,eval,symmap,info,NULL);
    }

    /* Destroy the last evaluated condition */
    JE_VCALL(&condlast,destroy);

    /* Return the last evaluated statement */
    return stmtlast;
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_MAP JE_Symbols(int set) {
    JE_MAP map = JE_MapCreate(0);

    if(set & JE_SYMBOLS_CONST) JE_MapSet(&map, JE_StrCreate(JE_Strdup("VERSION")), JE_ValueStr(JE_StrCreate(JE_Strdup(JE_VERSION))));
    if(set & JE_SYMBOLS_CONST) JE_MapSet(&map, JE_StrCreate(JE_Strdup("VERSION_MAJOR")), JE_ValueI64(JE_VERSION_MAJOR));
    if(set & JE_SYMBOLS_CONST) JE_MapSet(&map, JE_StrCreate(JE_Strdup("VERSION_MINOR")), JE_ValueI64(JE_VERSION_MINOR));
    if(set & JE_SYMBOLS_CONST) JE_MapSet(&map, JE_StrCreate(JE_Strdup("VERSION_PATCH")), JE_ValueI64(JE_VERSION_PATCH));

    if(set & JE_SYMBOLS_IO)    JE_MapSet(&map, JE_StrCreate(JE_Strdup("PRINT")), JE_ValueFun(JE_FunCreate(JE_Strdup("PRINT"), _PRINT, JE_Strdup("*"), NULL, NULL)));

    if(set & JE_SYMBOLS_LINEAR) JE_MapSet(&map, JE_StrCreate(JE_Strdup("BINARY")), JE_ValueFun(JE_FunCreate(JE_Strdup("BINARY"), _BINARY, JE_Strdup("I"), NULL, NULL)));
    if(set & JE_SYMBOLS_LINEAR) JE_MapSet(&map, JE_StrCreate(JE_Strdup("CEIL")), JE_ValueFun(JE_FunCreate(JE_Strdup("CEIL"), _CEIL, JE_Strdup("#"), NULL, NULL)));
    if(set & JE_SYMBOLS_LINEAR) JE_MapSet(&map, JE_StrCreate(JE_Strdup("FLOOR")), JE_ValueFun(JE_FunCreate(JE_Strdup("FLOOR"), _FLOOR, JE_Strdup("#"), NULL, NULL)));
    if(set & JE_SYMBOLS_LINEAR) JE_MapSet(&map, JE_StrCreate(JE_Strdup("HEX")), JE_ValueFun(JE_FunCreate(JE_Strdup("HEX"), _HEX, JE_Strdup("I"), NULL, NULL)));
    if(set & JE_SYMBOLS_LINEAR) JE_MapSet(&map, JE_StrCreate(JE_Strdup("IF")), JE_ValueFun(JE_FunCreate(JE_Strdup("IF"), _IF, JE_Strdup("..**"), NULL, NULL)));
    if(set & JE_SYMBOLS_LINEAR) JE_MapSet(&map, JE_StrCreate(JE_Strdup("LEN")), JE_ValueFun(JE_FunCreate(JE_Strdup("LEN"), _LEN, JE_Strdup("?"), NULL, NULL)));
    if(set & JE_SYMBOLS_LINEAR) JE_MapSet(&map, JE_StrCreate(JE_Strdup("ROUND")), JE_ValueFun(JE_FunCreate(JE_Strdup("ROUND"), _ROUND, JE_Strdup("#"), NULL, NULL)));
    if(set & JE_SYMBOLS_LINEAR) JE_MapSet(&map, JE_StrCreate(JE_Strdup("SQRT")), JE_ValueFun(JE_FunCreate(JE_Strdup("SQRT"), _SQRT, JE_Strdup("#"), NULL, NULL)));
    if(set & JE_SYMBOLS_LINEAR) JE_MapSet(&map, JE_StrCreate(JE_Strdup("STRLEN")), JE_ValueFun(JE_FunCreate(JE_Strdup("STRLEN"), _STRLEN, JE_Strdup("S"), NULL, NULL)));

    if(set & JE_SYMBOLS_LOOPABLE) JE_MapSet(&map, JE_StrCreate(JE_Strdup("FOR")), JE_ValueFun(JE_FunCreate(JE_Strdup("FOR"), _FOR, JE_Strdup("...."), NULL, NULL)));
    if(set & JE_SYMBOLS_LOOPABLE) JE_MapSet(&map, JE_StrCreate(JE_Strdup("FOREACH")), JE_ValueFun(JE_FunCreate(JE_Strdup("FOREACH"), _FOREACH, JE_Strdup(".?."), NULL, NULL)));
    if(set & JE_SYMBOLS_LOOPABLE) JE_MapSet(&map, JE_StrCreate(JE_Strdup("FUNCTION")), JE_ValueFun(JE_FunCreate(JE_Strdup("FUNCTION"), _FUNCTION, JE_Strdup(".."), NULL, NULL)));
    if(set & JE_SYMBOLS_LOOPABLE) JE_MapSet(&map, JE_StrCreate(JE_Strdup("WHILE")), JE_ValueFun(JE_FunCreate(JE_Strdup("WHILE"), _WHILE, JE_Strdup(".."), NULL, NULL)));

    if(set & JE_SYMBOLS_SYSTEM)   JE_MapSet(&map, JE_StrCreate(JE_Strdup("EVAL")), JE_ValueFun(JE_FunCreate(JE_Strdup("EVAL"), _EVAL, JE_Strdup("?"), NULL, NULL)));

    return map;
}
