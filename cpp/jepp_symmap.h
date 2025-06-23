#ifndef JEPP_SYMMAP_H_
#define JEPP_SYMMAP_H_

#include <memory>
#include <string>
#include "jepp_value.h"

namespace je {


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_MAP JE_MAP;


/* ***************************************************************************
* TYPES
*/

class Symmap {
    JE_MAP* wsymmap;

    public:
        Symmap();
        Symmap(const std::string& symbols);
        ~Symmap();

        Value operator[](const char* key) const;
        Value operator[](const std::string& key) const;

    friend class Parsed;
};


} /* namespace je */

#endif /* JEPP_SYMMAP_H_ */
