#include <iostream>
#include "jepp/jepp.h"

int main() {
    auto instance = je::instance();
    auto symmap = instance.symmap(R"je({
        "grades" : {
            "alice"   : [ "A", "B", "C" ],
            "bob"     : [ "B", "C", "D" ],
            "charlie" : [ "C", "F", "A" ],
        }
    })je");
    auto grades = symmap["grades"];

    std::cout << "grades  : " << grades << std::endl;
    std::cout << "size    : " << grades.size() << std::endl;
    std::cout << std::endl;

    grades["bob"] = je::Value("F");
    grades["charlie"][1] = je::Value("D");
    grades["david"] = je::Value("No grade");

    std::cout << "alice   : " << grades["alice"] << std::endl;
    std::cout << "bob     : " << grades["bob"] << std::endl;
    std::cout << "charlie : " << grades["charlie"] << std::endl;
    std::cout << std::endl;

    return 0;
}
