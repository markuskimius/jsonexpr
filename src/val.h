#ifndef VAL_H_
#define VAL_H_

#include <inttypes.h>


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct MAP MAP;
typedef struct VEC VEC;
typedef struct FUNC FUNC;
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
    FUNCTION_V  = 'F',
    NODE_V      = 'N',
} val_t;


/* ***************************************************************************
* TYPES
*/

typedef struct VAL {
    val_t type;

    union {
        int64_t i;
        double f;
        char* s;
        VEC* v;
        MAP* m;
        FUNC* fn;
        NODE* n;
    } value;

    char* astrdecoded;
    char* astrencoded;
} VAL;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VAL* nullval();
VAL* boolval(int64_t i64);
VAL* intval(int64_t i64);
VAL* dblval(double f64);
VAL* strval(char* str);
VAL* arrval(VEC* vec);
VAL* objval(MAP* map);
VAL* funcval(FUNC* func);
VAL* nodeval(NODE* node);
VAL* dupval(VAL* val);
void swapval(VAL* val1, VAL* val2);
void freeval(VAL* val);

int valtrue(VAL* val);
int valcmp(VAL* val1, VAL* val2);

char* valstr(VAL* val);
char* valqstr(VAL* val);
const char* valtype(VAL* val);


/* ***************************************************************************
* EXPORTED FUNCTIONS
*/

int gettype(VAL* val);
int64_t getint64(VAL* val);
double getdouble(VAL* val);
char* getstring(VAL* val);
VEC* getarray(VAL* val);
MAP* getobject(VAL* val);
FUNC* getbuiltin(VAL* val);


#endif /* VAL_H_ */
