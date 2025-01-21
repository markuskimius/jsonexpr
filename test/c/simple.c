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
