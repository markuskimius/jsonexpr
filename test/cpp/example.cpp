#include <iostream>
#include "jepp/jepp.h"

int main() {
    auto instance = je::instance();
    auto parsed = instance.parse(R"je(
        PRINT("I have " + LEN(grades) + " students");
        PRINT("Alice's grade is " + grades.alice);
        grades
    )je");
    auto symmap = instance.symmap(R"je({
        "grades" : {
            "alice"   : "A",
            "bob"     : "B",
            "charlie" : "B",
        }
    })je");
    auto result = parsed.eval(symmap);

    return 0;
}
