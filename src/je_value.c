#include <stdlib.h>
#include "je_ref.h"
#include "je_type.h"
#include "je_util.h"
#include "je_value.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_VALUE JE_ValueNul() {
    JE_VALUE value;

    value.type = &JE_typeNul;
    value.data.i64 = 0;

    return value;
}

JE_VALUE JE_ValueTru(int tru) {
    JE_VALUE value;

    value.type = &JE_typeTru;
    value.data.tru = tru ? 1 : 0;

    return value;
}

JE_VALUE JE_ValueI64(int64_t i64) {
    JE_VALUE value;

    value.type = &JE_typeI64;
    value.data.i64 = i64;

    return value;
}

JE_VALUE JE_ValueF64(double f64) {
    JE_VALUE value;

    value.type = &JE_typeF64;
    value.data.f64 = f64;

    return value;
}

JE_VALUE JE_ValueStr(JE_STR str) {
    JE_VALUE value;

    value.type = &JE_typeStr;
    value.data.str = str;

    return value;
}

JE_VALUE JE_ValueMap(JE_MAP map) {
    JE_VALUE value;

    value.type = &JE_typeMap;
    value.data.map = JE_RefMapAlloc(map);

    return value;
}

JE_VALUE JE_ValueVec(JE_VEC vec) {
    JE_VALUE value;

    value.type = &JE_typeVec;
    value.data.vec = JE_RefVecAlloc(vec);

    return value;
}

JE_VALUE JE_ValueFun(JE_FUN fun) {
    JE_VALUE value;

    value.type = &JE_typeFun;
    value.data.fun = JE_RefFunAlloc(fun);

    return value;
}

JE_VALUE JE_ValueNod(JE_NOD nod) {
    JE_VALUE value;

    value.type = &JE_typeNod;
    value.data.nod = nod;

    return value;
}

JE_VALUE JE_ValueRefMap(JE_MAP* map) {
    JE_VALUE value;

    value.type = &JE_typeMap;
    value.data.map = map;

    return value;
}

JE_VALUE JE_ValueRefVec(JE_VEC* vec) {
    JE_VALUE value;

    value.type = &JE_typeVec;
    value.data.vec = vec;

    return value;
}

JE_VALUE JE_ValueRefFun(JE_FUN* fun) {
    JE_VALUE value;

    value.type = &JE_typeFun;
    value.data.fun = fun;

    return value;
}
