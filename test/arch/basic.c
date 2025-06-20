#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "je_map.h"
#include "je_str.h"
#include "je_type.h"
#include "je_value.h"
#include "je_vec.h"


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static JE_STR _StrCreate(const char* cstr) {
    return JE_StrCreate(strdup(cstr));
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

int main() {
    JE_VEC vec = JE_VecCreate(0);
    JE_VALUE v[] = {
        JE_ValueVec(JE_VecCreate(0)),
        JE_ValueVec(JE_VecCreate(0)),
        JE_ValueMap(JE_MapCreate(0)),
        JE_ValueMap(JE_MapCreate(0)),
        JE_ValueNul(),
        JE_ValueTru(0),
        JE_ValueTru(1),
        JE_ValueTru(-1),
        JE_ValueI64(0),
        JE_ValueI64(1234),
        JE_ValueI64(-1234),
        JE_ValueF64(0.0),
        JE_ValueF64(-0.0),
        JE_ValueF64(1.0),
        JE_ValueF64(12.34),
        JE_ValueF64(-12.34),
        JE_ValueF64(1.0 / 0.0),
        JE_ValueF64(-1.0 / 0.0),
        JE_ValueF64(0.0 / 0.0),
        JE_ValueF64(1.7976931348623157E+308),
        JE_ValueF64(-4.9406564584124654E-324),
        JE_ValueStr(_StrCreate("")),
        JE_ValueStr(_StrCreate("Hello, world!\1")),
    };
    size_t max = sizeof(v) / sizeof(*v);

    JE_VecPush(v[0].data.vec, JE_ValueStr(_StrCreate("Bye\n")));
    JE_VecPush(v[0].data.vec, (JE_ValueF64(3.1415)));
    JE_VecPush(v[0].data.vec, (JE_ValueStr(_StrCreate("world!"))));

    JE_MapSet(v[2].data.map, _StrCreate("Hello"), JE_ValueStr(_StrCreate("World!")));
    JE_MapSet(v[2].data.map, _StrCreate("Bye"), JE_ValueStr(_StrCreate("Cruel world!")));

    for(int i=0; i<max; i++) {
        JE_VALUE x = v[i];
        JE_VALUE y = JE_VCALL(&x, plus, x);
        JE_VALUE z = JE_VCALL(&x, clone);
        int tru = JE_VCALL(&x, toTru);
        JE_VALUE i64 = JE_VCALL(&x, toI64);
        JE_VALUE f64 = JE_VCALL(&x, toF64);
        char* i64s = JE_VCALL(&i64, toQstr);
        char* f64s = JE_VCALL(&f64, toQstr);
        char* strx = JE_VCALL(&x, toQstr);
        char* stry = JE_VCALL(&y, toQstr);
        char* strz = NULL;

        strz = JE_VCALL(&z, toQstr);

        printf("%s\n", strx);
        printf("plus = %s\n", stry);
        printf("tru  = %d\n", tru);
        printf("i64  = %s\n", i64s);
        printf("f64  = %s\n", f64s);
        printf("\n");

        JE_VCALL(&y,destroy);
        JE_VCALL(&z,destroy);
        JE_VCALL(&i64,destroy);
        JE_VCALL(&f64,destroy);
        free(i64s);
        free(f64s);
        free(strx);
        free(stry);
    }

    printf("----\n\n");

    for(int i=0; i<max; i++) {
        for(int j=0; j<max; j++) {
            JE_VALUE x = v[i];
            JE_VALUE y = v[j];
            JE_VALUE z = JE_VCALL(&x, plus, y);
            char* strx = JE_VCALL(&x, toQstr);
            char* stry = JE_VCALL(&y, toQstr);
            char* strz = JE_VCALL(&z, toQstr);

            printf("%s + %s = %s\n", strx, stry, strz);

            JE_VCALL(&z,destroy);
            free(strx);
            free(stry);
            free(strz);
        }
    }

    for(int i=0; i<max; i++) {
        JE_VALUE x = v[i];

        JE_VCALL(&x,destroy);
    }

    return 0;
}
