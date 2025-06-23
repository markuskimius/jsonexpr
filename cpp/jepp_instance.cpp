#include "jepp_instance.h"

namespace je {


/* ***************************************************************************
* METHODS
*/

Symmap Instance::symmap() {
    return Symmap();
}

Symmap Instance::symmap(const std::string& symbols) {
    return Symmap(symbols);
}

Parsed Instance::parse(const std::string& code) {
    return Parsed(code);
}


} /* namespace je */
