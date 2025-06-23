#ifndef JE_TYPE_H_
#define JE_TYPE_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_ASTINFO JE_ASTINFO;
typedef struct JE_MAP JE_MAP;
typedef struct JE_NODE JE_NODE;
typedef struct JE_STR JE_STR;
typedef struct JE_VALUE JE_VALUE;
typedef struct JE_VEC JE_VEC;


/* ***************************************************************************
* TYPES
*/

typedef enum {
    JE_NUL_T = '0',
    JE_TRU_T = 'B',
    JE_I64_T = 'I',
    JE_F64_T = 'F',
    JE_STR_T = 'S',
    JE_MAP_T = 'O',
    JE_VEC_T = 'A',
    JE_FUN_T = 'C',
    JE_NOD_T = 'N',
} JE_TYPE_T;

typedef struct JE_TYPE {
    JE_VALUE (*clone)(const JE_VALUE*);
    void (*destroy)(JE_VALUE*);
    JE_TYPE_T (*getType)(const JE_VALUE*);
    const char* (*getTypeCstr)(const JE_VALUE*);

    JE_VALUE (*_and)(const JE_VALUE*, const JE_VALUE);
    JE_VALUE (*asr)(const JE_VALUE*, const JE_VALUE);
    void (*assn)(JE_VALUE*, JE_VALUE);
    JE_VALUE (*call)(const JE_VALUE*, const JE_VEC, const JE_NODE*, JE_MAP*, const JE_ASTINFO*);
    int64_t (*compare)(const JE_VALUE*, const JE_VALUE);
    JE_VALUE (*divby)(const JE_VALUE*, const JE_VALUE);
    JE_VALUE (*inv)(const JE_VALUE*);
    JE_VALUE (*length)(const JE_VALUE*);
    JE_VALUE (*minus)(const JE_VALUE*, const JE_VALUE);
    JE_VALUE (*mod)(const JE_VALUE*, const JE_VALUE);
    JE_VALUE (*neg)(const JE_VALUE*);
    JE_VALUE (*_or)(const JE_VALUE*, const JE_VALUE);
    JE_VALUE (*plus)(const JE_VALUE*, const JE_VALUE);
    JE_VALUE (*pos)(const JE_VALUE*);
    JE_VALUE (*pow)(const JE_VALUE*, const JE_VALUE);
    JE_VALUE (*shl)(const JE_VALUE*, const JE_VALUE);
    JE_VALUE (*shr)(const JE_VALUE*, const JE_VALUE);
    JE_VALUE (*times)(const JE_VALUE*, const JE_VALUE);
    JE_VALUE (*_xor)(const JE_VALUE*, const JE_VALUE);
    void (*assnasr)(JE_VALUE*, const JE_VALUE);
    void (*assnbitand)(JE_VALUE*, const JE_VALUE);
    void (*assnbitor)(JE_VALUE*, const JE_VALUE);
    void (*assnbitxor)(JE_VALUE*, const JE_VALUE);
    void (*assndivby)(JE_VALUE*, const JE_VALUE);
    void (*assnminus)(JE_VALUE*, const JE_VALUE);
    void (*assnmod)(JE_VALUE*, const JE_VALUE);
    void (*assnplus)(JE_VALUE*, const JE_VALUE);
    void (*assnpow)(JE_VALUE*, const JE_VALUE);
    void (*assnshl)(JE_VALUE*, const JE_VALUE);
    void (*assnshr)(JE_VALUE*, const JE_VALUE);
    void (*assntimes)(JE_VALUE*, const JE_VALUE);

    JE_VALUE (*toI64)(const JE_VALUE*);
    JE_VALUE (*toF64)(const JE_VALUE*);
    int (*toTru)(const JE_VALUE*);
    char* (*toCstr)(const JE_VALUE*);
    char* (*toQstr)(const JE_VALUE*);
    JE_STR (*toStr)(const JE_VALUE*);
} JE_TYPE;


/* ***************************************************************************
* GLOBALS
*/

extern JE_TYPE JE_typeNul;
extern JE_TYPE JE_typeTru;
extern JE_TYPE JE_typeI64;
extern JE_TYPE JE_typeF64;
extern JE_TYPE JE_typeStr;
extern JE_TYPE JE_typeMap;
extern JE_TYPE JE_typeVec;
extern JE_TYPE JE_typeFun;
extern JE_TYPE JE_typeNod;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

const char* JE_TypeToCstr(JE_TYPE_T type);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_TYPE_H_ */
