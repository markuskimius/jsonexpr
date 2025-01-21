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
#include "je/je.h"

int main() {
    JE_VAL* result = NULL;
    JE_COMPILED* compiled = je_compile(
        "PRINT(\"I have \" + LEN(grades) + \" students\");"
        "PRINT(\"Alice's grade is \" + grades.alice);"
    );

    je_setSymbols(compiled,
        "{"
        "  \"grades\" : {"
        "    \"alice\" : \"A\","
        "    \"bob\"   : \"B\","
        "  }"
        "}"
    );

    result = je_evalc(compiled);

    JE_ValDelete(result);
    je_free(compiled);

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

