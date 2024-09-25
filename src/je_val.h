#ifndef JE_VAL_H_
#define JE_VAL_H_

#include <inttypes.h>


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
} val_t;


/* ***************************************************************************
* TYPES
*/

typedef struct JE_VAL {
    val_t type;

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

JE_VAL* je_nullval();
JE_VAL* je_boolval(int64_t i64);
JE_VAL* je_intval(int64_t i64);
JE_VAL* je_dblval(double f64);
JE_VAL* je_strval(char* str);
JE_VAL* je_arrval(JE_VEC* vec);
JE_VAL* je_objval(JE_MAP* map);
JE_VAL* je_funcval(JE_FUNC* func);
JE_VAL* je_nodeval(JE_NODE* node);
JE_VAL* je_dupval(JE_VAL* val);
void je_swapval(JE_VAL* val1, JE_VAL* val2);
void je_freeval(JE_VAL* val);

int je_valtrue(JE_VAL* val);
int je_valcmp(JE_VAL* val1, JE_VAL* val2);

char* je_valstr(JE_VAL* val);
char* je_valqstr(JE_VAL* val);
const char* je_valtype(JE_VAL* val);


/* ***************************************************************************
* EXPORTED FUNCTIONS
*/

int je_gettype(JE_VAL* val);
int64_t je_getint64(JE_VAL* val);
double je_getdouble(JE_VAL* val);
char* je_getstring(JE_VAL* val);
JE_VEC* je_getarray(JE_VAL* val);
JE_MAP* je_getobject(JE_VAL* val);
JE_FUNC* je_getfunc(JE_VAL* val);


#endif /* JE_VAL_H_ */
