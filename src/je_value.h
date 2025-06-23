#ifndef JE_VALUE_H_
#define JE_VALUE_H_

#include <inttypes.h>
#include "je_nod.h"
#include "je_str.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_FUN JE_FUN;
typedef struct JE_MAP JE_MAP;
typedef struct JE_TYPE JE_TYPE;
typedef struct JE_VEC JE_VEC;


/* ***************************************************************************
* TYPES
*/

typedef struct JE_VALUE {
    JE_TYPE* type;

    union {
        int     tru;
        int64_t i64;
        double  f64;
        JE_STR  str;
        JE_MAP* map;
        JE_VEC* vec;
        JE_FUN* fun;
        JE_NOD  nod;
    } data;
} JE_VALUE;


/* ***************************************************************************
* MACROS
*/

#define JE_VCALL(vp,method,...) ((vp)->type->method((vp),##__VA_ARGS__))


/* ***************************************************************************
* FUNCTIONS
*/

JE_VALUE JE_ValueNul();
JE_VALUE JE_ValueTru(int tru);
JE_VALUE JE_ValueI64(int64_t i64);
JE_VALUE JE_ValueF64(double f64);
JE_VALUE JE_ValueStr(JE_STR str);
JE_VALUE JE_ValueMap(JE_MAP map);
JE_VALUE JE_ValueVec(JE_VEC vec);
JE_VALUE JE_ValueFun(JE_FUN fun);
JE_VALUE JE_ValueNod(JE_NOD nod);
JE_VALUE JE_ValueRefMap(JE_MAP* map);
JE_VALUE JE_ValueRefVec(JE_VEC* vec);
JE_VALUE JE_ValueRefFun(JE_FUN* fun);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_VALUE_H_ */
