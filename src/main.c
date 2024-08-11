#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eval.h"
#include "map.h"
#include "util.h"
#include "parse.h"
#include "value.h"
#include "vector.h"

#define R(...) #__VA_ARGS__

int main() {
    NODE* ast = parse(R(
        memo = [ 0, 1 ];
        fib = FUNCTION("I",
            x = ARG[0];

            IF(x >= LEN(memo),
                x2 = fib(x-2);
                x1 = fib(x-1);

                memo[x] = x1 + x2
            );

            memo[x]
        );

        fib(9);
    ));

    VALUE* result = eval(ast, NULL);

    printf("%s\n", strencoded(result));
    freevalue(result);

    return 0;
}
