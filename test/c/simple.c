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
