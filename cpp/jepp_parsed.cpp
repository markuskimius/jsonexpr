#include <cstdlib>
#include "je/je.h"
#include "jepp_error.h"
#include "jepp_parsed.h"

namespace je {


/* ***************************************************************************
* METHODS
*/

Parsed::Parsed(const std::string& code) {
    this->wast = (JE_AST*) malloc(sizeof(JE_AST));
    *this->wast = JE_Parse(code.c_str());
}

Parsed::~Parsed() {
    JE_AstDestroy(this->wast);
    free(this->wast);
}

Value Parsed::eval(Symmap& symmap) {
    JE_VALUE wvalue = JE_AstEval(*this->wast, symmap.wsymmap, 0);

    return Value(wvalue);
}


} /* namespace je */
