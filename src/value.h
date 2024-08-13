#ifndef VALUE_H_
#define VALUE_H_

#include <inttypes.h>


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct MAP MAP;
typedef struct VEC VEC;
typedef struct FUNC FUNC;
typedef struct UFUNC UFUNC;
typedef struct NODE NODE;


/* ***************************************************************************
* CONSTANTS
*/

typedef enum {
    NULL_V      = '0',
    BOOL_V      = 'B',
    INT_V       = 'I',
    FLOAT_V     = 'D',
    STRING_V    = 'S',
    ARRAY_V     = 'A',
    OBJECT_V    = 'O',
    BUILTIN_V   = 'F',
    USERFUNC_V  = 'f',
    NODE_V      = 'N',
} value_t;


/* ***************************************************************************
* TYPES
*/

typedef struct VALUE {
    value_t type;
    size_t count;
    union {
        int64_t i;
        double f;
        char* s;
        VEC* v;
        MAP* m;
        FUNC* b;
        UFUNC* u;
        NODE* n;
    } value;
    char* astrdecoded;
    char* astrencoded;
} VALUE;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VALUE* nullvalue();
VALUE* boolvalue(int64_t i64);
VALUE* intvalue(int64_t i64);
VALUE* dblvalue(double f64);
VALUE* strvalue(char* str);
VALUE* arrvalue(VEC* vec);
VALUE* objvalue(MAP* map);
VALUE* bfnvalue(FUNC* func);
VALUE* ufnvalue(UFUNC* ufunc);
VALUE* nodevalue(NODE* node);
VALUE* dupvalue(VALUE* value);
void freevalue(VALUE* value);

char* strdecoded(VALUE* value);
char* strencoded(VALUE* value);
const char* valuetype(VALUE* value);

int istrue(VALUE* value);
int cmpvalue(VALUE* value1, VALUE* value2);


/* ***************************************************************************
* EXPORTED FUNCTIONS
*/

int gettype(VALUE* value);
int64_t getint64(VALUE* value);
double getdouble(VALUE* value);
char* getstring(VALUE* value);
VEC* getarray(VALUE* value);
MAP* getobject(VALUE* value);
FUNC* getbuiltin(VALUE* value);
UFUNC* getuserfunc(VALUE* value);


#endif /* VALUE_H_ */
