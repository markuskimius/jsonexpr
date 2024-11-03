# jsonexpr for C

This document describes using jsonexpr with C.
For the language overview, see the [main page](https://github.com/markuskimius/jsonexpr).


## Installation

```bash
$ make
$ sudo make PREFIX=/usr/local install
```


## Example

```c
#include <stdlib.h>
#include <sys/types.h>
#include "je/je.h"
#include "je/je_map.h"
#include "je/je_symtbl.h"

const char* code =
    "PRINT(\"I have \" + LEN(grades) + \" students\");\n"
    "PRINT(\"Alice's grade is \" + grades.alice);\n"
;

int main() {
    JE_NODE* ast = je_parse(code);
    JE_SYMTBL* symbols = je_newtable(NULL);
    JE_MAP* grades = je_newmap();
    JE_VAL* result = NULL;

    je_mapset(grades, "alice", je_strval("A"));
    je_mapset(grades, "bob", je_strval("B"));
    je_tableset(symbols, "grades", je_objval(grades), 0);

    result = je_eval(ast, symbols);

    je_freeval(result);
    je_freenode(ast);

    return 0;
}
```

Output:

```
I have 2 students
Alice's grade is A
```


## License

[Apache 2.0](https://github.com/markuskimius/jsonexpr/blob/main/LICENSE)

