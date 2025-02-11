#ifndef JE_VAL_H_
#define JE_VAL_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_FUNC JE_FUNC;
typedef struct JE_NODE JE_NODE;
typedef struct JE_MAP JE_MAP;
typedef struct JE_VAL JE_VAL;
typedef struct JE_VEC JE_VEC;


/* ***************************************************************************
* CONSTANTS
*/

typedef enum {
    JE_NULL_V      = '0',
    JE_BOOL_V      = 'B',
    JE_INT_V       = 'I',
    JE_FLOAT_V     = 'D',
    JE_STRING_V    = 'S',
    JE_ARRAY_V     = 'A',
    JE_OBJECT_V    = 'O',
    JE_FUNCTION_V  = 'F',
    JE_NODE_V      = 'N',
} JE_VAL_T;


/* ***************************************************************************
* TYPES
*/

typedef struct JE_VAL {
    JE_VAL_T type;

    union {
        int64_t i;
        double f;
        char* s;
        JE_VEC* v;
        JE_MAP* m;
        JE_FUNC* fn;
        JE_NODE* n;
        JE_VAL* r;
    } value;

    char* astrdecoded;
    char* astrencoded;
} JE_VAL;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_VAL* JE_ValNewFromNull();
JE_VAL* JE_ValNewFromBool(int64_t i64);
JE_VAL* JE_ValNewFromInt(int64_t i64);
JE_VAL* JE_ValNewFromFloat(double f64);
JE_VAL* JE_ValNewFromCstr(const char* str);
JE_VAL* JE_ValNewFromVec(JE_VEC* vec);
JE_VAL* JE_ValNewFromMap(JE_MAP* map);
JE_VAL* JE_ValNewFromFunc(JE_FUNC* func);
JE_VAL* JE_ValNewFromNode(JE_NODE* node);
JE_VAL* JE_ValDup(JE_VAL* val);
void JE_ValSwap(JE_VAL* val1, JE_VAL* val2);
void JE_ValDelete(JE_VAL* val);

int JE_ValIsTrue(JE_VAL* val);
int JE_ValCmp(JE_VAL* val1, JE_VAL* val2);

const char* JE_ValToCstr(JE_VAL* val);
const char* JE_ValToQstr(JE_VAL* val);
const char* JE_ValGetType(JE_VAL* val);


/* ***************************************************************************
* EXPORTED FUNCTIONS
*/

int64_t  JE_ValGetBool(JE_VAL* val);
int64_t  JE_ValGetInt(JE_VAL* val);
double   JE_ValGetFloat(JE_VAL* val);
char*    JE_ValGetString(JE_VAL* val);
JE_VEC*  JE_ValGetVec(JE_VAL* val);
JE_MAP*  JE_ValGetMap(JE_VAL* val);
JE_FUNC* JE_ValGetFunc(JE_VAL* val);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_VAL_H_ */
