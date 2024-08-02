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


/* ***************************************************************************
* CONSTANTS
*/

typedef enum {
    NULL_V      = 'N',
    INT64_V     = 'I',
    DOUBLE_V    = 'D',
    STRING_V    = 'S',
    ARRAY_V     = 'A',
    OBJECT_V    = 'O',
    BUILTIN_V   = 'F',
    USERFUNC_V  = 'f',
} value_t;


/* ***************************************************************************
* TYPES
*/

typedef struct VALUE {
    value_t type;
    size_t count;
    union {
        int64_t i64;
        double f64;
        char* str;
        char* name;
        MAP* map;
        VEC* vec;
        FUNC* bfn;
        UFUNC* ufn;
    } value;
    char* astrvalue;
    char* astrencoded;
} VALUE;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VALUE* newnull();
VALUE* newint64(int64_t i64);
VALUE* newdouble(double f64);
VALUE* newstring(const char* str);
VALUE* newarray(VEC* vec);
VALUE* newobject(MAP* map);
VALUE* newbuiltin(FUNC* func);
VALUE* newuserfunc(UFUNC* ufunc);
VALUE* dupvalue(VALUE* value);
void freevalue(VALUE* value);
char* strvalue(VALUE* value);
char* strencoded(VALUE* value);


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
