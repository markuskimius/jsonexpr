#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eval.h"
#include "map.h"
#include "util.h"
#include "parse.h"
#include "value.h"
#include "vector.h"

int main() {
    NODE* ast = parse(NULL);
    VALUE* result = eval(ast, NULL);
    char* buf = NULL;

    switch(result->type) {
        case NULL_V     : printf("NULL\n"); break;
        case INT64_V    : printf("i64 : %lld\n", result->value.i64); break;
        case DOUBLE_V   : printf("dbl : %lf\n", result->value.f64);  break;
        case STRING_V   : printf("str : %s\n", buf = astrencode(result->value.str)); break;
        case ARRAY_V    : printf("arr : %s\n", buf = astrvec(result->value.vec)); break;
        case OBJECT_V   : printf("obj : %s\n", buf = astrmap(result->value.map)); break;
        case BUILTIN_V  : printf("bfn : %s\n", buf = strdup("FUNCTION")); break;
        case USERFUNC_V : printf("ufn : %s\n", buf = strdup("FUNCTION")); break;
        default         : fprintf(stderr, "Invalid value type: '%c' (%d)\n", result->type, result->type); break;
    }

    if(buf) free(buf);

    return 0;
}
