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

    printf("%s\n", strencoded(result));
    freevalue(result);

    return 0;
}
