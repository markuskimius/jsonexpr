#include <iostream>
#include "jepp.h"

namespace je {


/* ***************************************************************************
* FUNCTIONS
*/

Instance instance() {
    Instance instance;

    return instance;
}

Value eval(const std::string& code) {
    Instance instance = Instance();
    Parsed parsed = instance.parse(code);
    Symmap symmap = instance.symmap();

    return eval(code, symmap);
}

Value eval(const std::string& code, Symmap& symmap) {
    Value value;

    return value;
}


} /* namespace je */
