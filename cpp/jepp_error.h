#ifndef JEPP_ERROR_H_
#define JEPP_ERROR_H_

#include <string>

namespace je {


/* ***************************************************************************
* TYPES
*/

class JeException {
    public:
        std::string text;

        JeException(const std::string& text);
};

class UsageError : public JeException {
    public:
        UsageError(const std::string& text);
};

class TypeError : public UsageError {
    public:
        TypeError(const std::string& text);
};

class IndexError : public UsageError {
    public:
        IndexError(const std::string& text);
};

class SyntaxError : public JeException {
    public:
        SyntaxError(const std::string& text);
};

class Exit : public JeException {
    public:
        Exit(const std::string& text);
};


} /* namespace je */

#endif /* JEPP_ERROR_H_ */
