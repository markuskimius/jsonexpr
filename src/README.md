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
    JE_COMPILED* compiled = JE_Compile(
        "PRINT(\"I have \" + LEN(grades) + \" students\");"
        "PRINT(\"Alice's grade is \" + grades.alice);"
    );

    JE_SetSymbols(compiled,
        "{"
        "  \"grades\" : {"
        "    \"alice\" : \"A\","
        "    \"bob\"   : \"B\","
        "  }"
        "}"
    );

    result = JE_EvalCompiled(compiled);

    JE_ValDelete(result);
    JE_FreeCompiled(compiled);

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

