#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "je_error.h"
#include "je_node.h"
#include "je_ntype.h"
#include "je_ref.h"
#include "je_token.h"
#include "je_type.h"
#include "je_util.h"
#include "je_value.h"


/* ***************************************************************************
* PRIVATE FUNCTIONS - NUL
*/

static JE_VALUE _NulClone(const JE_VALUE* x) {
    return JE_ValueNul();
}

static void _NulDestroy(JE_VALUE* x) {
    memset(x, 0, sizeof(JE_VALUE));
}

static JE_TYPE_T _NulGetType(const JE_VALUE* x) {
    return JE_NUL_T;
}

static const char* _NulGetTypeCstr(const JE_VALUE* x) {
    return "NULL";
}

static JE_VALUE _NulAnd(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _NulAsr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static void _NulAssn(JE_VALUE* x, JE_VALUE y) {
    JE_VCALL(x,destroy);
    *x = y;
}

static JE_VALUE _NulCall(const JE_VALUE* x, const JE_VEC y, const JE_NODE* source, JE_MAP* symmap, const JE_ASTINFO* info) {
    JE_RuntimeError(JE_NCALL(source,tokenBegin,info), JE_NCALL(source,tokenEnd,info), "Cannot call a NULL value");

    return JE_ValueNul();
}

static int64_t _NulCompare(const JE_VALUE* x, const JE_VALUE y) {
    int64_t result = 0;

    switch(JE_VCALL(&y,getType)) {
        case JE_NUL_T : result = 0; break;
        default       : result = -1; break;
    }

    return result;
}

static JE_VALUE _NulDivby(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _NulInv(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _NulLength(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _NulMinus(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _NulMod(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _NulNeg(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _NulOr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _NulPlus(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _NulPos(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _NulPow(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _NulShl(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _NulShr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _NulTimes(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static void _NulAssnAsr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NulAssnBitAnd(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NulAssnBitOr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NulAssnBitXor(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NulAssnDivby(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NulAssnMinus(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NulAssnMod(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NulAssnPlus(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NulAssnPow(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NulAssnShl(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NulAssnShr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NulAssnTimes(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static JE_VALUE _NulToI64(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _NulToF64(const JE_VALUE* x) {
    return JE_ValueNul();
}

static int _NulToCtru(const JE_VALUE* x) {
    return 0;
}

static char* _NulToCstr(const JE_VALUE* x) {
    return JE_Strdup("null");
}

static char* _NulToQstr(const JE_VALUE* x) {
    return _NulToCstr(x);
}

static JE_STR _NulToStr(const JE_VALUE* x) {
    return JE_StrCreate(JE_Strdup("null"));
}

static JE_VALUE _NulXor(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - TRU
*/

static JE_VALUE _TruClone(const JE_VALUE* x) {
    return JE_ValueTru(x->data.tru);
}

static void _TruDestroy(JE_VALUE* x) {
    memset(x, 0, sizeof(JE_VALUE));
}

static JE_TYPE_T _TruGetType(const JE_VALUE* x) {
    return JE_TRU_T;
}

static const char* _TruGetTypeCstr(const JE_VALUE* x) {
    return "BOOLEAN";
}

static JE_VALUE _TruAnd(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _TruAsr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static void _TruAssn(JE_VALUE* x, JE_VALUE y) {
    JE_VCALL(x,destroy);
    *x = y;
}

static JE_VALUE _TruCall(const JE_VALUE* x, const JE_VEC y, const JE_NODE* source, JE_MAP* symmap, const JE_ASTINFO* info) {
    JE_RuntimeError(JE_NCALL(source,tokenBegin,info), JE_NCALL(source,tokenEnd,info), "Cannot call a BOOLEAN value");

    return JE_ValueNul();
}

static int64_t _TruCompare(const JE_VALUE* x, const JE_VALUE y) {
    int64_t result = 0;

    switch(JE_VCALL(&y,getType)) {
        case JE_NUL_T : result = 1; break;
        case JE_TRU_T : result = x->data.tru - y.data.tru; break;
        default       : result = -1; break;
    }

    return result;
}

static JE_VALUE _TruDivby(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _TruInv(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _TruLength(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _TruMinus(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _TruMod(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _TruNeg(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _TruOr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _TruPlus(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _TruPos(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _TruPow(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _TruShl(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _TruShr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _TruTimes(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static void _TruAssnAsr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _TruAssnBitAnd(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _TruAssnBitOr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _TruAssnBitXor(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _TruAssnDivby(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _TruAssnMinus(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _TruAssnMod(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _TruAssnPlus(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _TruAssnPow(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _TruAssnShl(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _TruAssnShr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _TruAssnTimes(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static JE_VALUE _TruToI64(const JE_VALUE* x) {
    return JE_ValueI64(x->data.tru ? 1 : 0);
}

static JE_VALUE _TruToF64(const JE_VALUE* x) {
    return JE_ValueF64(x->data.tru ? 1.0 : 0.0);
}

static int _TruToCtru(const JE_VALUE* x) {
    return x->data.tru;
}

static char* _TruToCstr(const JE_VALUE* x) {
    return x->data.tru ? JE_Strdup("true") : JE_Strdup("false");
}

static char* _TruToQstr(const JE_VALUE* x) {
    return _TruToCstr(x);
}

static JE_STR _TruToStr(const JE_VALUE* x) {
    return JE_StrCreate(_TruToCstr(x));
}

static JE_VALUE _TruXor(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - I64
*/

static JE_VALUE _I64Clone(const JE_VALUE* x) {
    return JE_ValueI64(x->data.i64);
}

static void _I64Destroy(JE_VALUE* x) {
    memset(x, 0, sizeof(JE_VALUE));
}

static JE_TYPE_T _I64GetType(const JE_VALUE* x) {
    return JE_I64_T;
}

static const char* _I64GetTypeCstr(const JE_VALUE* x) {
    return "INT64";
}

static JE_VALUE _I64And(const JE_VALUE* x, const JE_VALUE y) {
    JE_VALUE result;

    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : result = JE_ValueI64(JE_VCALL(x,toI64).data.i64 & JE_VCALL(&y,toI64).data.i64); break;
        default       : result = JE_ValueNul(); break;
    }

    return result;
}

static JE_VALUE _I64Asr(const JE_VALUE* x, const JE_VALUE y) {
    JE_VALUE result;

    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : {
            int64_t a = x->data.i64;
            int64_t b = y.data.i64;

            if     (b <=-64         ) result = JE_ValueI64(0);
            else if(b <   0         ) result = JE_ValueI64(a << -b);
            else if(b ==  0         ) result = JE_ValueI64(a);
            else if(b <  64 && a < 0) result = JE_ValueI64(~(~a >> b));
            else if(b <  64         ) result = JE_ValueI64(a >> b);
            else if(           a < 0) result = JE_ValueI64(-1);
            else                      result = JE_ValueI64(0);

            break;
        }
        default       : result = JE_ValueNul(); break;
    }

    return result;
}

static void _I64Assn(JE_VALUE* x, JE_VALUE y) {
    JE_VCALL(x,destroy);
    *x = y;
}

static JE_VALUE _I64Call(const JE_VALUE* x, const JE_VEC y, const JE_NODE* source, JE_MAP* symmap, const JE_ASTINFO* info) {
    JE_RuntimeError(JE_NCALL(source,tokenBegin,info), JE_NCALL(source,tokenEnd,info), "Cannot call an INT64 value");

    return JE_ValueNul();
}

static int64_t _I64Compare(const JE_VALUE* x, const JE_VALUE y) {
    int64_t result = 0;

    switch(JE_VCALL(&y,getType)) {
        case JE_NUL_T : result = 1; break;
        case JE_TRU_T : result = 1; break;
        case JE_I64_T : result = (x->data.i64<y.data.i64) ? -1 : (x->data.i64==y.data.i64) ? 0 : 1; break;
        case JE_F64_T : result = (x->data.i64<y.data.f64) ? -1 : (x->data.i64==y.data.f64) ? 0 : 1; break;
        default       : result = -1; break;
    }

    return result;
}

static JE_VALUE _I64Divby(const JE_VALUE* x, const JE_VALUE y) {
    JE_VALUE result;

    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : result = JE_ValueI64(JE_VCALL(x,toI64).data.i64 / JE_VCALL(&y,toI64).data.i64); break;
        case JE_F64_T : result = JE_ValueF64(JE_VCALL(x,toF64).data.f64 / JE_VCALL(&y,toF64).data.f64); break;
        default       : result = JE_ValueNul(); break;
    }

    return result;
}

static JE_VALUE _I64Inv(const JE_VALUE* x) {
    return JE_ValueI64(~x->data.i64);
}

static JE_VALUE _I64Length(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _I64Minus(const JE_VALUE* x, const JE_VALUE y) {
    JE_VALUE result;

    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : result = JE_ValueI64(JE_VCALL(x,toI64).data.i64 - JE_VCALL(&y,toI64).data.i64); break;
        case JE_F64_T : result = JE_ValueF64(JE_VCALL(x,toF64).data.f64 - JE_VCALL(&y,toF64).data.f64); break;
        default       : result = JE_ValueNul(); break;
    }

    return result;
}

static JE_VALUE _I64Mod(const JE_VALUE* x, const JE_VALUE y) {
    JE_VALUE result;

    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : result = JE_ValueI64(JE_VCALL(x,toI64).data.i64 % JE_VCALL(&y,toI64).data.i64); break;
        default       : result = JE_ValueNul(); break;
    }

    return result;
}

static JE_VALUE _I64Neg(const JE_VALUE* x) {
    return JE_ValueI64(-x->data.i64);
}

static JE_VALUE _I64Or(const JE_VALUE* x, const JE_VALUE y) {
    JE_VALUE result;

    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : result = JE_ValueI64(JE_VCALL(x,toI64).data.i64 | JE_VCALL(&y,toI64).data.i64); break;
        default       : result = JE_ValueNul(); break;
    }

    return result;
}

static JE_VALUE _I64Plus(const JE_VALUE* x, const JE_VALUE y) {
    JE_VALUE result;

    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : result = JE_ValueI64(JE_VCALL(x,toI64).data.i64 + JE_VCALL(&y,toI64).data.i64); break;
        case JE_F64_T : result = JE_ValueF64(JE_VCALL(x,toF64).data.f64 + JE_VCALL(&y,toF64).data.f64); break;
        default       : result = JE_ValueNul(); break;
    }

    return result;
}

static JE_VALUE _I64Pos(const JE_VALUE* x) {
    return JE_ValueI64(x->data.i64);
}

static JE_VALUE _I64Pow(const JE_VALUE* x, const JE_VALUE y) {
    JE_VALUE result;

    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : /* fallthru */
        case JE_F64_T : result = JE_ValueF64(pow(JE_VCALL(x,toF64).data.f64, JE_VCALL(&y,toF64).data.f64)); break;
        default       : result = JE_ValueNul(); break;
    }

    return result;
}

static JE_VALUE _I64Shl(const JE_VALUE* x, const JE_VALUE y) {
    JE_VALUE result;

    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : {
            int64_t a = x->data.i64;
            int64_t b = y.data.i64;

            if     (b <=-64 && a < 0) result = JE_ValueI64(-1);
            else if(b <=-64         ) result = JE_ValueI64(0);
            else if(b <   0 && a < 0) result = JE_ValueI64(~(~a >> -b));
            else if(b <   0         ) result = JE_ValueI64(a >> -b);
            else if(b ==  0         ) result = JE_ValueI64(a);
            else if(b <  64         ) result = JE_ValueI64(a << b);
            else                      result = JE_ValueI64(0);

            break;
        }
        default       : result = JE_ValueNul(); break;
    }

    return result;
}

static JE_VALUE _I64Shr(const JE_VALUE* x, const JE_VALUE y) {
    JE_VALUE result;

    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : {
            uint64_t a = (uint64_t) x->data.i64;
            int64_t b = y.data.i64;

            if     (b <=-64) result = JE_ValueI64(0);
            else if(b <   0) result = JE_ValueI64(a << -b);
            else if(b ==  0) result = JE_ValueI64(a);
            else if(b <  64) result = JE_ValueI64(a >> b);
            else             result = JE_ValueI64(0);

            break;
        }
        default       : result = JE_ValueNul(); break;
    }

    return result;
}

static JE_VALUE _I64Times(const JE_VALUE* x, const JE_VALUE y) {
    JE_VALUE result;

    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : result = JE_ValueI64(JE_VCALL(x,toI64).data.i64 * JE_VCALL(&y,toI64).data.i64); break;
        case JE_F64_T : result = JE_ValueF64(JE_VCALL(x,toF64).data.f64 * JE_VCALL(&y,toF64).data.f64); break;
        default       : result = JE_ValueNul(); break;
    }

    return result;
}

static void _I64AssnAsr(JE_VALUE* x, const JE_VALUE y) {
    if(y.type == &JE_typeI64) {
        int64_t a = x->data.i64;
        int64_t b = y.data.i64;

        if     (b <=-64         ) x->data.i64 = 0;
        else if(b <   0         ) x->data.i64 <<= -b;
        else if(b ==  0         ) x->data.i64 = a;
        else if(b <  64 && a < 0) x->data.i64 = ~(~a >> b);
        else if(b <  64         ) x->data.i64 >>= b;
        else if(           a < 0) x->data.i64 = -1;
        else                      x->data.i64 = 0;
    }
}

static void _I64AssnBitAnd(JE_VALUE* x, const JE_VALUE y) {
    if(y.type == &JE_typeI64) {
        x->data.i64 &= y.data.i64;
    }
}

static void _I64AssnBitOr(JE_VALUE* x, const JE_VALUE y) {
    if(y.type == &JE_typeI64) {
        x->data.i64 |= y.data.i64;
    }
}

static void _I64AssnBitXor(JE_VALUE* x, const JE_VALUE y) {
    if(y.type == &JE_typeI64) {
        x->data.i64 ^= y.data.i64;
    }
}

static void _I64AssnDivby(JE_VALUE* x, const JE_VALUE y) {
    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : x->data.i64 /= y.data.i64; break;
        case JE_F64_T : *x = JE_ValueF64(x->data.i64 / y.data.f64); break;
        default       : /* Nothing to do */ break;
    }
}

static void _I64AssnMinus(JE_VALUE* x, const JE_VALUE y) {
    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : x->data.i64 -= y.data.i64; break;
        case JE_F64_T : *x = JE_ValueF64(x->data.i64 - y.data.f64); break;
        default       : /* Nothing to do */ break;
    }
}

static void _I64AssnMod(JE_VALUE* x, const JE_VALUE y) {
    if(y.type == &JE_typeI64) {
        x->data.i64 %= y.data.i64;
    }
}

static void _I64AssnPlus(JE_VALUE* x, const JE_VALUE y) {
    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : x->data.i64 += y.data.i64; break;
        case JE_F64_T : *x = JE_ValueF64(x->data.i64 + y.data.f64); break;
        default       : /* Nothing to do */ break;
    }
}

static void _I64AssnPow(JE_VALUE* x, const JE_VALUE y) {
    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : *x = JE_ValueF64(pow(x->data.i64, y.data.i64)); break;
        case JE_F64_T : *x = JE_ValueF64(pow(x->data.i64, y.data.f64)); break;
        default       : /* Nothing to do */ break;
    }
}

static void _I64AssnShl(JE_VALUE* x, const JE_VALUE y) {
    if(y.type == &JE_typeI64) {
        int64_t a = x->data.i64;
        int64_t b = y.data.i64;

        if     (b <=-64 && a < 0) x->data.i64 = -1;
        else if(b <=-64         ) x->data.i64 = 0;
        else if(b <   0 && a < 0) x->data.i64 = (~(~a >> -b));
        else if(b <   0         ) x->data.i64 >>= -b;
        else if(b ==  0         ) x->data.i64 = a;
        else if(b <  64         ) x->data.i64 <<= b;
        else                      x->data.i64 = 0;
    }
}

static void _I64AssnShr(JE_VALUE* x, const JE_VALUE y) {
    if(y.type == &JE_typeI64) {
        uint64_t a = (uint64_t) x->data.i64;
        int64_t b = y.data.i64;

        if     (b <=-64) x->data.i64 = 0;
        else if(b <   0) x->data.i64 = a << -b;
        else if(b ==  0) x->data.i64 = a;
        else if(b <  64) x->data.i64 = a >> b;
        else             x->data.i64 = 0;
    }
}

static void _I64AssnTimes(JE_VALUE* x, const JE_VALUE y) {
    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : x->data.i64 *= y.data.i64; break;
        case JE_F64_T : *x = JE_ValueF64(x->data.i64 * y.data.f64); break;
        default       : /* Nothing to do */ break;
    }
}

static JE_VALUE _I64ToI64(const JE_VALUE* x) {
    return JE_ValueI64(x->data.i64);
}

static JE_VALUE _I64ToF64(const JE_VALUE* x) {
    return JE_ValueF64((double) x->data.i64);
}

static int _I64ToCtru(const JE_VALUE* x) {
    return x->data.i64 ? 1 : 0;
}

static char* _I64ToCstr(const JE_VALUE* x) {
    char* buf = JE_Malloc(32);

    snprintf(buf, 32, "%lld", x->data.i64);

    return buf;
}

static char* _I64ToQstr(const JE_VALUE* x) {
    return _I64ToCstr(x);
}

static JE_STR _I64ToStr(const JE_VALUE* x) {
    return JE_StrCreate(_I64ToCstr(x));
}

static JE_VALUE _I64Xor(const JE_VALUE* x, const JE_VALUE y) {
    JE_VALUE result;

    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : result = JE_ValueI64(JE_VCALL(x,toI64).data.i64 ^ JE_VCALL(&y,toI64).data.i64); break;
        default       : result = JE_ValueNul(); break;
    }

    return result;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - F64
*/

static JE_VALUE _F64Clone(const JE_VALUE* x) {
    return JE_ValueF64(x->data.f64);
}

static void _F64Destroy(JE_VALUE* x) {
    memset(x, 0, sizeof(JE_VALUE));
}

static JE_TYPE_T _F64GetType(const JE_VALUE* x) {
    return JE_F64_T;
}

static const char* _F64GetTypeCstr(const JE_VALUE* x) {
    return "FLOAT64";
}

static JE_VALUE _F64And(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _F64Asr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static void _F64Assn(JE_VALUE* x, JE_VALUE y) {
    JE_VCALL(x,destroy);
    *x = y;
}

static JE_VALUE _F64Call(const JE_VALUE* x, const JE_VEC y, const JE_NODE* source, JE_MAP* symmap, const JE_ASTINFO* info) {
    JE_RuntimeError(JE_NCALL(source,tokenBegin,info), JE_NCALL(source,tokenEnd,info), "Cannot call a FLOAT64 value");

    return JE_ValueNul();
}

static JE_VALUE _F64Divby(const JE_VALUE* x, const JE_VALUE y) {
    JE_VALUE result;

    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : /* passthru */
        case JE_F64_T : result = JE_ValueF64(JE_VCALL(x,toF64).data.f64 / JE_VCALL(&y,toF64).data.f64); break;
        default       : result = JE_ValueNul(); break;
    }

    return result;
}

static JE_VALUE _F64Inv(const JE_VALUE* x) {
    return JE_ValueNul();
}

static int64_t _F64Compare(const JE_VALUE* x, const JE_VALUE y) {
    int64_t result = 0;

    switch(JE_VCALL(&y,getType)) {
        case JE_NUL_T : result = 1; break;
        case JE_TRU_T : result = 1; break;
        case JE_I64_T : result = (x->data.f64<y.data.i64) ? -1 : (x->data.f64==y.data.i64) ? 0 : 1; break;
        case JE_F64_T : result = (x->data.f64<y.data.f64) ? -1 : (x->data.f64==y.data.f64) ? 0 : 1; break;
        default       : result = -1; break;
    }

    return result;
}

static JE_VALUE _F64Length(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _F64Minus(const JE_VALUE* x, const JE_VALUE y) {
    JE_VALUE result;

    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : /* passthru */
        case JE_F64_T : result = JE_ValueF64(JE_VCALL(x,toF64).data.f64 - JE_VCALL(&y,toF64).data.f64); break;
        default       : result = JE_ValueNul(); break;
    }

    return result;
}

static JE_VALUE _F64Mod(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _F64Neg(const JE_VALUE* x) {
    return JE_ValueF64(-x->data.f64);
}

static JE_VALUE _F64Or(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _F64Plus(const JE_VALUE* x, const JE_VALUE y) {
    JE_VALUE result;

    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : /* passthru */
        case JE_F64_T : result = JE_ValueF64(JE_VCALL(x,toF64).data.f64 + JE_VCALL(&y,toF64).data.f64); break;
        default       : result = JE_ValueNul(); break;
    }

    return result;
}

static JE_VALUE _F64Pos(const JE_VALUE* x) {
    return JE_ValueF64(x->data.f64);
}

static JE_VALUE _F64Pow(const JE_VALUE* x, const JE_VALUE y) {
    JE_VALUE result;

    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : /* fallthru */
        case JE_F64_T : result = JE_ValueF64(pow(JE_VCALL(x,toF64).data.f64, JE_VCALL(&y,toF64).data.f64)); break;
        default       : result = JE_ValueNul(); break;
    }

    return result;
}

static JE_VALUE _F64Shl(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _F64Shr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _F64Times(const JE_VALUE* x, const JE_VALUE y) {
    JE_VALUE result;

    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : /* passthru */
        case JE_F64_T : result = JE_ValueF64(JE_VCALL(x,toF64).data.f64 * JE_VCALL(&y,toF64).data.f64); break;
        default       : result = JE_ValueNul(); break;
    }

    return result;
}

static void _F64AssnAsr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _F64AssnBitAnd(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _F64AssnBitOr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _F64AssnBitXor(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _F64AssnDivby(JE_VALUE* x, const JE_VALUE y) {
    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : x->data.f64 /= y.data.i64; break;
        case JE_F64_T : x->data.f64 /= y.data.f64; break;
        default       : /* Nothing to do */ break;
    }
}

static void _F64AssnMinus(JE_VALUE* x, const JE_VALUE y) {
    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : x->data.f64 -= y.data.i64; break;
        case JE_F64_T : x->data.f64 -= y.data.f64; break;
        default       : /* Nothing to do */ break;
    }
}

static void _F64AssnMod(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _F64AssnPlus(JE_VALUE* x, const JE_VALUE y) {
    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : x->data.f64 += y.data.i64; break;
        case JE_F64_T : x->data.f64 += y.data.f64; break;
        default       : *x = JE_ValueNul(); break;
    }
}

static void _F64AssnPow(JE_VALUE* x, const JE_VALUE y) {
    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : x->data.f64 = pow(x->data.f64, y.data.i64); break;
        case JE_F64_T : x->data.f64 = pow(x->data.f64, y.data.f64); break;
        default       : /* Nothing to do */ break;
    }
}

static void _F64AssnShl(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _F64AssnShr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _F64AssnTimes(JE_VALUE* x, const JE_VALUE y) {
    switch(JE_VCALL(&y,getType)) {
        case JE_I64_T : x->data.f64 *= y.data.i64; break;
        case JE_F64_T : x->data.f64 *= y.data.f64; break;
        default       : /* Nothing to do */ break;
    }
}

static JE_VALUE _F64ToI64(const JE_VALUE* x) {
    return JE_ValueI64((int64_t) x->data.f64);
}

static JE_VALUE _F64ToF64(const JE_VALUE* x) {
    return JE_ValueF64(x->data.f64);
}

static int _F64ToCtru(const JE_VALUE* x) {
    return x->data.f64 ? 1 : 0;
}

static char* _F64ToCstr(const JE_VALUE* x) {
    return JE_F64ToCstr(x->data.f64);
}

static char* _F64ToQstr(const JE_VALUE* x) {
    return JE_F64ToQstr(x->data.f64);
}

static JE_STR _F64ToStr(const JE_VALUE* x) {
    return JE_StrCreate(_F64ToCstr(x));
}

static JE_VALUE _F64Xor(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - JE_STR
*/

static JE_VALUE _StrClone(const JE_VALUE* x) {
    return JE_ValueStr(JE_StrClone(x->data.str));
}

static void _StrDestroy(JE_VALUE* x) {
    JE_StrDestroy(&x->data.str);
    memset(x, 0, sizeof(JE_VALUE));
}

static JE_TYPE_T _StrGetType(const JE_VALUE* x) {
    return JE_STR_T;
}

static const char* _StrGetTypeCstr(const JE_VALUE* x) {
    return "STRING";
}

static JE_VALUE _StrAnd(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _StrAsr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static void _StrAssn(JE_VALUE* x, JE_VALUE y) {
    JE_VCALL(x,destroy);
    *x = y;
}

static JE_VALUE _StrCall(const JE_VALUE* x, const JE_VEC y, const JE_NODE* source, JE_MAP* symmap, const JE_ASTINFO* info) {
    JE_RuntimeError(JE_NCALL(source,tokenBegin,info), JE_NCALL(source,tokenEnd,info), "Cannot call a STRING value");

    return JE_ValueNul();
}

static int64_t _StrCompare(const JE_VALUE* x, const JE_VALUE y) {
    int64_t result = 0;

    switch(JE_VCALL(&y,getType)) {
        case JE_NUL_T : result = 1; break;
        case JE_TRU_T : result = 1; break;
        case JE_I64_T : result = 1; break;
        case JE_F64_T : result = 1; break;
        case JE_STR_T : result = JE_StrCompare(x->data.str, y.data.str); break;
        default       : result = -1; break;
    }

    return result;
}

static JE_VALUE _StrDivby(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _StrInv(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _StrLength(const JE_VALUE* x) {
    return JE_ValueI64(x->data.str.length);
}

static JE_VALUE _StrMinus(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _StrMod(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _StrNeg(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _StrOr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _StrPlus(const JE_VALUE* x, const JE_VALUE y) {
    JE_STR ystr = JE_VCALL(&y,toStr);
    JE_VALUE result = JE_ValueStr(JE_StrPlusStr(x->data.str, ystr));

    JE_StrDestroy(&ystr);

    return result;
}

static JE_VALUE _StrPos(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _StrPow(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _StrShl(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _StrShr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _StrTimes(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static void _StrAssnAsr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _StrAssnBitAnd(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _StrAssnBitOr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _StrAssnBitXor(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _StrAssnDivby(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _StrAssnMinus(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _StrAssnMod(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _StrAssnPlus(JE_VALUE* x, const JE_VALUE y) {
    switch(JE_VCALL(&y,getType)) {
        case JE_STR_T : JE_StrPushStr(&x->data.str, y.data.str); break;
        default       : {
            JE_STR ystr = JE_VCALL(&y,toStr);

            JE_StrPushStr(&x->data.str, ystr);
            JE_StrDestroy(&ystr);
        }
    }
}

static void _StrAssnPow(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _StrAssnShl(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _StrAssnShr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _StrAssnTimes(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static JE_VALUE _StrToI64(const JE_VALUE* x) {
    return JE_ValueI64(JE_StrToI64(x->data.str));
}

static JE_VALUE _StrToF64(const JE_VALUE* x) {
    return JE_ValueF64(JE_StrToF64(x->data.str));
}

static int _StrToCtru(const JE_VALUE* x) {
    return x->data.str.length ? 1 : 0;
}

static char* _StrToCstr(const JE_VALUE* x) {
    return JE_StrToCstr(x->data.str);
}

static char* _StrToQstr(const JE_VALUE* x) {
    return JE_StrToQstr(x->data.str);
}

static JE_STR _StrToStr(const JE_VALUE* x) {
    return JE_StrClone(x->data.str);
}

static JE_VALUE _StrXor(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - JE_MAP
*/

static JE_VALUE _MapClone(const JE_VALUE* x) {
    return JE_ValueRefMap(JE_RefMapDup(x->data.map));
}

static void _MapDestroy(JE_VALUE* x) {
    JE_RefMapFree(x->data.map);
    memset(x, 0, sizeof(JE_VALUE));
}

static JE_TYPE_T _MapGetType(const JE_VALUE* x) {
    return JE_MAP_T;
}

static const char* _MapGetTypeCstr(const JE_VALUE* x) {
    return "OBJECT";
}

static JE_VALUE _MapAnd(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _MapAsr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static void _MapAssn(JE_VALUE* x, JE_VALUE y) {
    JE_VCALL(x,destroy);
    *x = y;
}

static JE_VALUE _MapCall(const JE_VALUE* x, const JE_VEC y, const JE_NODE* source, JE_MAP* symmap, const JE_ASTINFO* info) {
    JE_RuntimeError(JE_NCALL(source,tokenBegin,info), JE_NCALL(source,tokenEnd,info), "Cannot call an OBJECT value");

    return JE_ValueNul();
}

static int64_t _MapCompare(const JE_VALUE* x, const JE_VALUE y) {
    int64_t result = 0;

    switch(JE_VCALL(&y,getType)) {
        case JE_NUL_T : result = 1; break;
        case JE_TRU_T : result = 1; break;
        case JE_I64_T : result = 1; break;
        case JE_F64_T : result = 1; break;
        case JE_STR_T : result = 1; break;
        case JE_MAP_T : result = JE_MapCompare(*x->data.map, *y.data.map); break;
        default       : result = -1; break;
    }

    return result;
}

static JE_VALUE _MapDivby(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _MapInv(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _MapLength(const JE_VALUE* x) {
    return JE_ValueI64(x->data.map->size);
}

static JE_VALUE _MapMinus(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _MapMod(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _MapNeg(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _MapOr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _MapPlus(const JE_VALUE* x, const JE_VALUE y) {
    JE_VALUE result;

    switch(JE_VCALL(&y,getType)) {
        case JE_MAP_T : result = JE_ValueMap(JE_MapPlusMap(*x->data.map, *y.data.map)); break;
        default       : result = JE_ValueNul(); break;
    }

    return result;
}

static JE_VALUE _MapPos(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _MapPow(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _MapShl(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _MapShr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _MapTimes(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static void _MapAssnAsr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _MapAssnBitAnd(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _MapAssnBitOr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _MapAssnBitXor(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _MapAssnDivby(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _MapAssnMinus(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _MapAssnMod(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _MapAssnPlus(JE_VALUE* x, const JE_VALUE y) {
    switch(JE_VCALL(&y,getType)) {
        case JE_MAP_T : JE_MapPlusAssnMap(x->data.map, *y.data.map); break;
        default       : JE_VCALL(x,destroy); *x = JE_ValueNul(); break;
    }
}

static void _MapAssnPow(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _MapAssnShl(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _MapAssnShr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _MapAssnTimes(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static JE_VALUE _MapToI64(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _MapToF64(const JE_VALUE* x) {
    return JE_ValueNul();
}

static int _MapToCtru(const JE_VALUE* x) {
    return JE_MapSize(*x->data.map) ? 1 : 0;
}

static char* _MapToCstr(const JE_VALUE* x) {
    return JE_MapToCstr(*x->data.map);
}

static char* _MapToQstr(const JE_VALUE* x) {
    return JE_MapToQstr(*x->data.map);
}

static JE_STR _MapToStr(const JE_VALUE* x) {
    return JE_StrCreate(_MapToCstr(x));
}

static JE_VALUE _MapXor(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - JE_VEC
*/

static JE_VALUE _VecClone(const JE_VALUE* x) {
    return JE_ValueRefVec(JE_RefVecDup(x->data.vec));
}

static void _VecDestroy(JE_VALUE* x) {
    JE_RefVecFree(x->data.vec);
    memset(x, 0, sizeof(JE_VALUE));
}

static JE_TYPE_T _VecGetType(const JE_VALUE* x) {
    return JE_VEC_T;
}

static const char* _VecGetTypeCstr(const JE_VALUE* x) {
    return "ARRAY";
}

static JE_VALUE _VecAnd(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _VecAsr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static void _VecAssn(JE_VALUE* x, JE_VALUE y) {
    JE_VCALL(x,destroy);
    *x = y;
}

static JE_VALUE _VecCall(const JE_VALUE* x, const JE_VEC y, const JE_NODE* source, JE_MAP* symmap, const JE_ASTINFO* info) {
    JE_RuntimeError(JE_NCALL(source,tokenBegin,info), JE_NCALL(source,tokenEnd,info), "Cannot call an ARRAY value");

    return JE_ValueNul();
}

static int64_t _VecCompare(const JE_VALUE* x, const JE_VALUE y) {
    int64_t result = 0;

    switch(JE_VCALL(&y,getType)) {
        case JE_NUL_T : result = 1; break;
        case JE_TRU_T : result = 1; break;
        case JE_I64_T : result = 1; break;
        case JE_F64_T : result = 1; break;
        case JE_STR_T : result = 1; break;
        case JE_MAP_T : result = 1; break;
        case JE_VEC_T : result = JE_VecCompare(*x->data.vec, *y.data.vec); break;
        default       : result = -1; break;
    }

    return result;
}

static JE_VALUE _VecDivby(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _VecInv(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _VecLength(const JE_VALUE* x) {
    return JE_ValueI64(x->data.vec->length);
}

static JE_VALUE _VecMinus(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _VecMod(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _VecNeg(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _VecOr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _VecPlus(const JE_VALUE* x, const JE_VALUE y) {
    JE_VALUE result;

    switch(JE_VCALL(&y,getType)) {
        case JE_VEC_T : result = JE_ValueVec(JE_VecPlusVec(*x->data.vec, *y.data.vec)); break;
        default       : result = JE_ValueNul(); break;
    }

    return result;
}

static JE_VALUE _VecPos(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _VecPow(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _VecShl(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _VecShr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _VecTimes(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static void _VecAssnAsr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _VecAssnBitAnd(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _VecAssnBitOr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _VecAssnBitXor(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _VecAssnDivby(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _VecAssnMinus(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _VecAssnMod(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _VecAssnPlus(JE_VALUE* x, const JE_VALUE y) {
    switch(JE_VCALL(&y,getType)) {
        case JE_VEC_T : JE_VecPlusAssnVec(x->data.vec, *y.data.vec); break;
        default       : JE_VCALL(x,destroy); *x = JE_ValueNul(); break;
    }
}

static void _VecAssnPow(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _VecAssnShl(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _VecAssnShr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _VecAssnTimes(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static JE_VALUE _VecToI64(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _VecToF64(const JE_VALUE* x) {
    return JE_ValueNul();
}

static int _VecToCtru(const JE_VALUE* x) {
    return JE_VecLength(*x->data.vec) ? 1 : 0;
}

static char* _VecToCstr(const JE_VALUE* x) {
    return JE_VecToCstr(*x->data.vec);
}

static char* _VecToQstr(const JE_VALUE* x) {
    return JE_VecToQstr(*x->data.vec);
}

static JE_STR _VecToStr(const JE_VALUE* x) {
    return JE_StrCreate(_VecToCstr(x));
}

static JE_VALUE _VecXor(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - JE_FUN
*/

static JE_VALUE _FunClone(const JE_VALUE* x) {
    return JE_ValueRefFun(JE_RefFunDup(x->data.fun));
}

static void _FunDestroy(JE_VALUE* x) {
    JE_RefFunFree(x->data.fun);
    memset(x, 0, sizeof(JE_VALUE));
}

static JE_TYPE_T _FunGetType(const JE_VALUE* x) {
    return JE_FUN_T;
}

static const char* _FunGetTypeCstr(const JE_VALUE* x) {
    return "FUNCTION";
}

static JE_VALUE _FunAnd(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _FunAsr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static void _FunAssn(JE_VALUE* x, JE_VALUE y) {
    JE_VCALL(x,destroy);
    *x = y;
}

static JE_VALUE _FunCall(const JE_VALUE* x, const JE_VEC y, const JE_NODE* source, JE_MAP* symmap, const JE_ASTINFO* info) {
    JE_VALUE result = JE_FunCall(*x->data.fun, y, source, symmap, info);

    return result;
}

static int64_t _FunCompare(const JE_VALUE* x, const JE_VALUE y) {
    int64_t result = 0;

    switch(JE_VCALL(&y,getType)) {
        case JE_NUL_T : result = 1; break;
        case JE_TRU_T : result = 1; break;
        case JE_I64_T : result = 1; break;
        case JE_F64_T : result = 1; break;
        case JE_STR_T : result = 1; break;
        case JE_MAP_T : result = 1; break;
        case JE_VEC_T : result = 1; break;
        case JE_FUN_T : result = strcmp(x->data.fun->name, y.data.fun->name); break;
        default       : result = -1; break;
    }

    return result;
}

static JE_VALUE _FunDivby(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _FunInv(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _FunLength(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _FunMinus(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _FunMod(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _FunNeg(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _FunOr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _FunPlus(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _FunPos(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _FunPow(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _FunShl(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _FunShr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _FunTimes(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static void _FunAssnAsr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _FunAssnBitAnd(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _FunAssnBitOr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _FunAssnBitXor(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _FunAssnDivby(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _FunAssnMinus(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _FunAssnMod(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _FunAssnPlus(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _FunAssnPow(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _FunAssnShl(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _FunAssnShr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _FunAssnTimes(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static JE_VALUE _FunToI64(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _FunToF64(const JE_VALUE* x) {
    return JE_ValueNul();
}

static int _FunToCtru(const JE_VALUE* x) {
    return 1;
}

static char* _FunToCstr(const JE_VALUE* x) {
    return JE_FunToCstr(*x->data.fun);
}

static char* _FunToQstr(const JE_VALUE* x) {
    return JE_FunToQstr(*x->data.fun);
}

static JE_STR _FunToStr(const JE_VALUE* x) {
    return JE_StrCreate(_FunToCstr(x));
}

static JE_VALUE _FunXor(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - JE_NODE
*/

static JE_VALUE _NodClone(const JE_VALUE* x) {
    return JE_ValueNod(x->data.nod);
}

static void _NodDestroy(JE_VALUE* x) {
    memset(x, 0, sizeof(JE_VALUE));
}

static JE_TYPE_T _NodGetType(const JE_VALUE* x) {
    return JE_NOD_T;
}

static const char* _NodGetTypeCstr(const JE_VALUE* x) {
    return "NODE";
}

static JE_VALUE _NodAnd(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _NodAsr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static void _NodAssn(JE_VALUE* x, JE_VALUE y) {
    JE_VCALL(x,destroy);
    *x = y;
}

static JE_VALUE _NodCall(const JE_VALUE* x, const JE_VEC y, const JE_NODE* source, JE_MAP* symmap, const JE_ASTINFO* info) {
    JE_RuntimeError(JE_NCALL(source,tokenBegin,info), JE_NCALL(source,tokenEnd,info), "Cannot call a NODE value");

    return JE_ValueNul();
}

static int _NodEqual(const JE_VALUE* x, const JE_VALUE y, const JE_NODE* source, const JE_ASTINFO* info) {
    int result = 0;

    switch(JE_VCALL(&y,getType)) {
        case JE_NOD_T : result = (x->data.nod.node->tmi == y.data.nod.node->tmi); break;
        default       : result = 0; break;
    }

    return result;
}

static int64_t _NodCompare(const JE_VALUE* x, const JE_VALUE y) {
    int64_t result = 0;

    switch(JE_VCALL(&y,getType)) {
        case JE_NUL_T : result = 1; break;
        case JE_TRU_T : result = 1; break;
        case JE_I64_T : result = 1; break;
        case JE_F64_T : result = 1; break;
        case JE_STR_T : result = 1; break;
        case JE_MAP_T : result = 1; break;
        case JE_VEC_T : result = 1; break;
        case JE_FUN_T : result = 1; break;
        case JE_NOD_T : result = x->data.nod.node->tmi - y.data.nod.node->tmi; break;
        default       : result = -1; break;
    }

    return result;
}

static JE_VALUE _NodDivby(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _NodInv(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _NodLength(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _NodMinus(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _NodMod(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _NodNeg(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _NodOr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _NodPlus(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _NodPos(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _NodPow(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _NodShl(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _NodShr(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static JE_VALUE _NodTimes(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}

static void _NodAssnAsr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NodAssnBitAnd(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NodAssnBitOr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NodAssnBitXor(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NodAssnDivby(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NodAssnMinus(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NodAssnMod(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NodAssnPlus(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NodAssnPow(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NodAssnShl(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NodAssnShr(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static void _NodAssnTimes(JE_VALUE* x, const JE_VALUE y) {
    *x = JE_ValueNul();
}

static JE_VALUE _NodToI64(const JE_VALUE* x) {
    return JE_ValueNul();
}

static JE_VALUE _NodToF64(const JE_VALUE* x) {
    return JE_ValueNul();
}

static int _NodToCtru(const JE_VALUE* x) {
    return 1;
}

static char* _NodToCstr(const JE_VALUE* x) {
    const JE_NODE* node = x->data.nod.node;
    const JE_ASTINFO* info = x->data.nod.info;

    return JE_TokenRangeToCstr(JE_NCALL(node,tokenBegin,info), JE_NCALL(node,tokenEnd,info));
}

static char* _NodToQstr(const JE_VALUE* x) {
    const JE_NODE* node = x->data.nod.node;
    const JE_ASTINFO* info = x->data.nod.info;

    return JE_TokenRangeToQstr(JE_NCALL(node,tokenBegin,info), JE_NCALL(node,tokenEnd,info));
}

static JE_STR _NodToStr(const JE_VALUE* x) {
    return JE_StrCreate(_NodToCstr(x));
}

static JE_VALUE _NodXor(const JE_VALUE* x, const JE_VALUE y) {
    return JE_ValueNul();
}


/* ***************************************************************************
* GLOBALS
*/

JE_TYPE JE_typeNul = { _NulClone, _NulDestroy, _NulGetType, _NulGetTypeCstr,   _NulAnd, _NulAsr, _NulAssn, _NulCall, _NulCompare, _NulDivby, _NulInv, _NulLength, _NulMinus, _NulMod, _NulNeg, _NulOr, _NulPlus, _NulPos, _NulPow, _NulShl, _NulShr, _NulTimes, _NulXor,   _NulAssnAsr, _NulAssnBitAnd, _NulAssnBitOr, _NulAssnBitXor, _NulAssnDivby, _NulAssnMinus, _NulAssnMod, _NulAssnPlus, _NulAssnPow, _NulAssnShl, _NulAssnShr, _NulAssnTimes,   _NulToI64, _NulToF64, _NulToCtru, _NulToCstr, _NulToQstr, _NulToStr };
JE_TYPE JE_typeTru = { _TruClone, _TruDestroy, _TruGetType, _TruGetTypeCstr,   _TruAnd, _TruAsr, _TruAssn, _TruCall, _TruCompare, _TruDivby, _TruInv, _TruLength, _TruMinus, _TruMod, _TruNeg, _TruOr, _TruPlus, _TruPos, _TruPow, _TruShl, _TruShr, _TruTimes, _TruXor,   _TruAssnAsr, _TruAssnBitAnd, _TruAssnBitOr, _TruAssnBitXor, _TruAssnDivby, _TruAssnMinus, _TruAssnMod, _TruAssnPlus, _TruAssnPow, _TruAssnShl, _TruAssnShr, _TruAssnTimes,   _TruToI64, _TruToF64, _TruToCtru, _TruToCstr, _TruToQstr, _TruToStr };
JE_TYPE JE_typeI64 = { _I64Clone, _I64Destroy, _I64GetType, _I64GetTypeCstr,   _I64And, _I64Asr, _I64Assn, _I64Call, _I64Compare, _I64Divby, _I64Inv, _I64Length, _I64Minus, _I64Mod, _I64Neg, _I64Or, _I64Plus, _I64Pos, _I64Pow, _I64Shl, _I64Shr, _I64Times, _I64Xor,   _I64AssnAsr, _I64AssnBitAnd, _I64AssnBitOr, _I64AssnBitXor, _I64AssnDivby, _I64AssnMinus, _I64AssnMod, _I64AssnPlus, _I64AssnPow, _I64AssnShl, _I64AssnShr, _I64AssnTimes,   _I64ToI64, _I64ToF64, _I64ToCtru, _I64ToCstr, _I64ToQstr, _I64ToStr };
JE_TYPE JE_typeF64 = { _F64Clone, _F64Destroy, _F64GetType, _F64GetTypeCstr,   _F64And, _F64Asr, _F64Assn, _F64Call, _F64Compare, _F64Divby, _F64Inv, _F64Length, _F64Minus, _F64Mod, _F64Neg, _F64Or, _F64Plus, _F64Pos, _F64Pow, _F64Shl, _F64Shr, _F64Times, _F64Xor,   _F64AssnAsr, _F64AssnBitAnd, _F64AssnBitOr, _F64AssnBitXor, _F64AssnDivby, _F64AssnMinus, _F64AssnMod, _F64AssnPlus, _F64AssnPow, _F64AssnShl, _F64AssnShr, _F64AssnTimes,   _F64ToI64, _F64ToF64, _F64ToCtru, _F64ToCstr, _F64ToQstr, _F64ToStr };
JE_TYPE JE_typeStr = { _StrClone, _StrDestroy, _StrGetType, _StrGetTypeCstr,   _StrAnd, _StrAsr, _StrAssn, _StrCall, _StrCompare, _StrDivby, _StrInv, _StrLength, _StrMinus, _StrMod, _StrNeg, _StrOr, _StrPlus, _StrPos, _StrPow, _StrShl, _StrShr, _StrTimes, _StrXor,   _StrAssnAsr, _StrAssnBitAnd, _StrAssnBitOr, _StrAssnBitXor, _StrAssnDivby, _StrAssnMinus, _StrAssnMod, _StrAssnPlus, _StrAssnPow, _StrAssnShl, _StrAssnShr, _StrAssnTimes,   _StrToI64, _StrToF64, _StrToCtru, _StrToCstr, _StrToQstr, _StrToStr };
JE_TYPE JE_typeMap = { _MapClone, _MapDestroy, _MapGetType, _MapGetTypeCstr,   _MapAnd, _MapAsr, _MapAssn, _MapCall, _MapCompare, _MapDivby, _MapInv, _MapLength, _MapMinus, _MapMod, _MapNeg, _MapOr, _MapPlus, _MapPos, _MapPow, _MapShl, _MapShr, _MapTimes, _MapXor,   _MapAssnAsr, _MapAssnBitAnd, _MapAssnBitOr, _MapAssnBitXor, _MapAssnDivby, _MapAssnMinus, _MapAssnMod, _MapAssnPlus, _MapAssnPow, _MapAssnShl, _MapAssnShr, _MapAssnTimes,   _MapToI64, _MapToF64, _MapToCtru, _MapToCstr, _MapToQstr, _MapToStr };
JE_TYPE JE_typeVec = { _VecClone, _VecDestroy, _VecGetType, _VecGetTypeCstr,   _VecAnd, _VecAsr, _VecAssn, _VecCall, _VecCompare, _VecDivby, _VecInv, _VecLength, _VecMinus, _VecMod, _VecNeg, _VecOr, _VecPlus, _VecPos, _VecPow, _VecShl, _VecShr, _VecTimes, _VecXor,   _VecAssnAsr, _VecAssnBitAnd, _VecAssnBitOr, _VecAssnBitXor, _VecAssnDivby, _VecAssnMinus, _VecAssnMod, _VecAssnPlus, _VecAssnPow, _VecAssnShl, _VecAssnShr, _VecAssnTimes,   _VecToI64, _VecToF64, _VecToCtru, _VecToCstr, _VecToQstr, _VecToStr };
JE_TYPE JE_typeFun = { _FunClone, _FunDestroy, _FunGetType, _FunGetTypeCstr,   _FunAnd, _FunAsr, _FunAssn, _FunCall, _FunCompare, _FunDivby, _FunInv, _FunLength, _FunMinus, _FunMod, _FunNeg, _FunOr, _FunPlus, _FunPos, _FunPow, _FunShl, _FunShr, _FunTimes, _FunXor,   _FunAssnAsr, _FunAssnBitAnd, _FunAssnBitOr, _FunAssnBitXor, _FunAssnDivby, _FunAssnMinus, _FunAssnMod, _FunAssnPlus, _FunAssnPow, _FunAssnShl, _FunAssnShr, _FunAssnTimes,   _FunToI64, _FunToF64, _FunToCtru, _FunToCstr, _FunToQstr, _FunToStr };
JE_TYPE JE_typeNod = { _NodClone, _NodDestroy, _NodGetType, _NodGetTypeCstr,   _NodAnd, _NodAsr, _NodAssn, _NodCall, _NodCompare, _NodDivby, _NodInv, _NodLength, _NodMinus, _NodMod, _NodNeg, _NodOr, _NodPlus, _NodPos, _NodPow, _NodShl, _NodShr, _NodTimes, _NodXor,   _NodAssnAsr, _NodAssnBitAnd, _NodAssnBitOr, _NodAssnBitXor, _NodAssnDivby, _NodAssnMinus, _NodAssnMod, _NodAssnPlus, _NodAssnPow, _NodAssnShl, _NodAssnShr, _NodAssnTimes,   _NodToI64, _NodToF64, _NodToCtru, _NodToCstr, _NodToQstr, _NodToStr };


/* ***************************************************************************
* FUNCTIONS
*/

const char* JE_TypeToCstr(JE_TYPE_T type) {
    const char* cstr = NULL;

    switch(type) {
        case JE_NUL_T : cstr = JE_typeNul.getTypeCstr(NULL); break;
        case JE_TRU_T : cstr = JE_typeTru.getTypeCstr(NULL); break;
        case JE_I64_T : cstr = JE_typeI64.getTypeCstr(NULL); break;
        case JE_F64_T : cstr = JE_typeF64.getTypeCstr(NULL); break;
        case JE_STR_T : cstr = JE_typeStr.getTypeCstr(NULL); break;
        case JE_MAP_T : cstr = JE_typeMap.getTypeCstr(NULL); break;
        case JE_VEC_T : cstr = JE_typeVec.getTypeCstr(NULL); break;
        case JE_FUN_T : cstr = JE_typeFun.getTypeCstr(NULL); break;
        case JE_NOD_T : cstr = JE_typeNod.getTypeCstr(NULL); break;
    }

    return cstr;
}
