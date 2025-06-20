#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "je_error.h"
#include "je_fun.h"
#include "je_node.h"
#include "je_ntype.h"
#include "je_ref.h"
#include "je_type.h"
#include "je_util.h"
#include "je_value.h"
#include "je_vec.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

/**
* Create a new JE_FUN.
*
* name and argfmt are allocated C strings.  Once passed into the function, they
* will be owned by JE_FUN and freed when JE_FUN is destroyed.
*/
JE_FUN JE_FunCreate(char* name, JE_FUNCALL call, char* argfmt, const JE_NODE* code, JE_MAP* symmap) {
    JE_FUN fun;

    fun.name = name;
    fun.argfmt = argfmt;
    fun.code = code;
    fun.call = call;
    fun.symmap = symmap ? JE_RefMapDup(symmap) : NULL;

    return fun;
}

/**
* Destroy the JE_FUN.
*/
void JE_FunDestroy(JE_FUN* fun) {
    if(fun->name) JE_Free(fun->name);
    if(fun->argfmt) JE_Free(fun->argfmt);
    if(fun->symmap) JE_RefMapFree(fun->symmap);

    memset(fun, 0, sizeof(JE_FUN));
}

/**
* Call the JE_FUN.
*/
JE_VALUE JE_FunCall(const JE_FUN fun, const JE_VEC args, const JE_NODE* source, JE_MAP* symmap, const JE_ASTINFO* info) {
    const char* fp = fun.argfmt;
    JE_VEC* refargs = JE_RefVecAlloc(JE_VecCreate(args.length));
    JE_VALUE result = JE_ValueNul();
    int isok = 1;
    size_t i = 0;

    for(i=0; i<args.length && *fp; i++) {
        JE_VALUE value = args.data[i];
        const JE_NODE* node = value.data.nod.node;
        int translate = 0;
        int validate = 0;
        int next = *(fp+1);

        switch(*fp) {
            case JE_NUL_T :                 /* fallthru */
            case JE_TRU_T :                 /* fallthru */
            case JE_I64_T :                 /* fallthru */
            case JE_F64_T :                 /* fallthru */
            case JE_STR_T :                 /* fallthru */
            case JE_MAP_T :                 /* fallthru */
            case JE_VEC_T :                 /* fallthru */
            case JE_FUN_T :                 /* fallthru */
            case JE_NOD_T : validate = 1;   /* fallthru */

            /* These must be in this order: */
            case '#'      :                 /* fallthru */
            case '*'      : if(*fp == '*' && next == '*') break;
            case '?'      : translate = 1;  /* fallthru */
            case '.'      : break;

            default       :
                JE_RuntimeError(JE_NCALL(node,tokenBegin,info), JE_NCALL(node,tokenEnd,info), "Invalid function argument specifier '%c' at position %zu while calling function '%s'", *fp, i, fun.name);
        }

        if(translate) {
            value = JE_NCALL(node,eval,symmap,info,NULL);
        }

        if(validate && JE_VCALL(&value,getType) != *fp) {
            JE_RuntimeError(JE_NCALL(node,tokenBegin,info), JE_NCALL(node,tokenEnd,info), "%s expected, got %s", JE_TypeToCstr(*fp), JE_VCALL(&value,getTypeCstr));
            isok = 0;
        }

        if(*fp == '#' && value.type != &JE_typeI64 && value.type != &JE_typeF64) {
            JE_RuntimeError(JE_NCALL(node,tokenBegin,info), JE_NCALL(node,tokenEnd,info), "INT64 or FLOAT64 expected, got %s", JE_VCALL(&value,getTypeCstr));
            isok = 0;
        }

        JE_VecPush(refargs, JE_VCALL(&value,clone));

        if(*fp != '*') fp++;
    }

    /* Check argument count */
    if(isok && *fp != '\0' && *fp != '*') {
        size_t minlen = i;

        while(*fp && *fp != '*') {
            minlen++;
            fp++;
        }

        JE_RuntimeError(JE_NCALL(source,tokenBegin,info), JE_NCALL(source,tokenEnd,info), "Too few arguments (expected at least %zu, got %zu)", minlen, i);
    }
    else if(isok && i == args.length) {
        result = fun.call(refargs, fun.code, fun.symmap?fun.symmap:symmap, info);
    }
    else if(isok) {
        JE_RuntimeError(JE_NCALL(source,tokenBegin,info), JE_NCALL(source,tokenEnd,info), "Too many arguments (expected %zu, got %zu)", i, args.length);
    }

    JE_RefVecFree(refargs);

    return result;
}

/**
* Convert JE_FUN to a C string.  The returned C string must be freed.
*/
char* JE_FunToCstr(const JE_FUN fun) {
    return strdup(fun.name);
}

/**
* Convert JE_FUN to a quoted C string.  The returned quoted C string must be
* freed.
*/
char* JE_FunToQstr(const JE_FUN fun) {
    return strdup(fun.name);
}
