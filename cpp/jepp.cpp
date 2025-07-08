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

Value eval(const std::string& code, Symmap& symmap) {
    Instance instance = Instance();
    Parsed parsed = instance.parse(code);

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

Value evalfile(const std::string& path, Symmap& symmap) {
    std::ifstream file(path);
    std::string line;
    std::string code;

    while(std::getline(file, line)) {
        code += line + "\n";
    }

    return eval(code, symmap);
}


} /* namespace je */
