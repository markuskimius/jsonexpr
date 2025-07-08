#ifndef JEPP_H_
#define JEPP_H_

#include <string>
#include "jepp_error.h"
#include "jepp_instance.h"
#include "jepp_iterator.h"
#include "jepp_parsed.h"
#include "jepp_symmap.h"
#include "jepp_value.h"

namespace je {


/* ***************************************************************************
* FUNCTIONS
*/

Instance instance();
Value eval(const std::string& code);
Value eval(const std::string& code, Symmap& symmap);
Value evalfile(const std::string& path);
Value evalfile(const std::string& path, Symmap& symmap);


} /* namespace je */

#endif /* JEPP_H_ */
