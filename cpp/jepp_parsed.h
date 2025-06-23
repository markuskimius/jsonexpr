#ifndef JEPP_PARSED_H_
#define JEPP_PARSED_H_

#include <memory>
#include <string>
#include "jepp_symmap.h"
#include "jepp_value.h"

namespace je {


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_AST JE_AST;


/* ***************************************************************************
* TYPES
*/

class Parsed {
    JE_AST* wast;

    public:
        Parsed(const std::string& code);
        ~Parsed();

        Value eval(Symmap& symmap);
};


} /* namespace je */

#endif /* JEPP_PARSED_H_ */
