#include <iostream>
#include "jepp/jepp.h"

int main() {
    auto instance = je::instance();
    auto symmap = instance.symmap(R"je({
        "mymap" : {
            "alice"   : "A",
            "bob"     : "B",
            "charlie" : "C",
        },
        "myvec" : [
            "alice",
            "bob",
            "charlie",
        ],
    })je");

    auto mymap = symmap["mymap"];
    auto myvec = symmap["mymap"];

    std::cout << "mymap:" << std::endl;
    for(auto i=symmap["mymap"].begin(); i!=symmap["mymap"].end(); i++) {
        std::cout << "(" << i.key() << ", " << i.value() << ")" << std::endl;
    }
    std::cout << std::endl;

    std::cout << "myvec:" << std::endl;
    for(auto i=symmap["myvec"].begin(); i!=symmap["myvec"].end(); i++) {
        std::cout << *i << std::endl;
    }
    std::cout << std::endl;

    return 0;
}
