#include <iostream>
#include "jepp_error.h"

namespace je {


/* ***************************************************************************
* METHODS
*/

JeException::JeException(const std::string& text) {
    this->text = text;
}

UsageError::UsageError(const std::string& text) : JeException(text) {
}

TypeError::TypeError(const std::string& text) : UsageError(text) {
}

IndexError::IndexError(const std::string& text) : UsageError(text) {
}

SyntaxError::SyntaxError(const std::string& text) : JeException(text) {
}

Exit::Exit(const std::string& text) : JeException(text) {
}


} /* namespace je */
