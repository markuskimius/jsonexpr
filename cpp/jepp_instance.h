#ifndef JEPP_INSTANCE_H_
#define JEPP_INSTANCE_H_

#include <memory>
#include <string>
#include "jepp_parsed.h"
#include "jepp_symmap.h"

namespace je {


/* ***************************************************************************
* TYPES
*/

class Instance {
    public:
        Symmap symmap();
        Symmap symmap(const std::string& symbols);
        Parsed parse(const std::string& code);
};


} /* namespace je */

#endif /* JEPP_INSTANCE_H_ */
