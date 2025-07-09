#include <fstream>
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

    return parsed.eval(symmap);
}

Value eval(const std::string& code, const std::string& symbols) {
    Instance instance = Instance();
    Parsed parsed = instance.parse(code);
    Symmap symmap = instance.symmap(symbols);

    return parsed.eval(symmap);
}

Value evalfile(const std::string& path) {
    std::ifstream file(path);
    std::string line;
    std::string code;

    while(std::getline(file, line)) {
        code += line + "\n";
    }

    return eval(code);
}

Value evalfile(const std::string& path, const std::string& symbols) {
    std::ifstream file(path);
    std::string line;
    std::string code;

    while(std::getline(file, line)) {
        code += line + "\n";
    }

    return eval(code, symbols);
}


} /* namespace je */
