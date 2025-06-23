#ifndef JEPP_ITERATOR_H_
#define JEPP_ITERATOR_H_

#include <string>

namespace je {


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

class Value;


/* ***************************************************************************
* TYPES
*/

class Iterator {
    struct JE_VALUE* wvalue;
    size_t index;

    public:
        Iterator(const struct JE_VALUE& wvalue, size_t index);
        ~Iterator();

        std::string key() const;
        Value value() const;

        Value operator*() const;
        Iterator& operator++();
        Iterator& operator++(int);
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
};


} /* namespace je */

#endif /* JEPP_ITERATOR_H_ */
