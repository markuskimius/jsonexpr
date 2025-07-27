# JSONexpr for C

This document describes using JSONexpr with C.
For the language overview, see the [main page](../README.md).


## Installation

```bash
$ make
$ sudo make PREFIX=/usr/local install
```


## Example

Code:
```c
#include <string.h>
#include "je/je.h"

int main() {
    JE_AST ast = JE_Parse(
        "PRINT(\"I have \" + LEN(grades) + \" students\");"
        "PRINT(\"Alice's grade is \" + grades.alice);"
    );
    JE_MAP grades = JE_MapCreate(0);
    JE_MAP* symmap = JE_SymmapCreate(JE_SYMBOLS_DEFAULT);
    JE_VALUE result;

    JE_MapSet(&grades, JE_StrCreate(strdup("alice")), JE_ValueStr(JE_StrCreate(strdup("A"))));
    JE_MapSet(&grades, JE_StrCreate(strdup("bob")), JE_ValueStr(JE_StrCreate(strdup("B"))));
    JE_MapSet(symmap, JE_StrCreate(strdup("grades")), JE_ValueMap(grades));

    result = JE_AstEval(ast, symmap, 0);

    JE_VCALL(&result,destroy);
    JE_SymmapDestroy(symmap);
    JE_AstDestroy(&ast);

    return 0;
}
```

Compilation:
```bash
$ clang -I/usr/local/include example.c -c
$ clang -L/usr/local/lib example.o -lje -o example
```

Output:
```
I have 2 students
Alice's grade is A
```


## License

[Apache 2.0](../LICENSE)
