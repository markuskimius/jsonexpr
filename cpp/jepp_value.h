#ifndef JEPP_VALUE_H_
#define JEPP_VALUE_H_

#include <iostream>
#include <string>

namespace je {


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

class Iterator;


/* ***************************************************************************
* TYPES
*/

class Value {
    struct JE_VALUE* wvalue;
    bool isref;

    public:
        Value();
        Value(bool tru);
        Value(int64_t i64);
        Value(double f64);
        Value(const char* cstr);
        Value(const std::string& str);
        Value(const struct JE_VALUE& wvalue);
        Value(struct JE_VALUE* wvalue);
        Value(const Value& value);
        Value(Value&& value);
        ~Value();

        size_t size() const;
        Iterator begin();
        Iterator end();

        Value& operator=(const Value&);
        Value operator[](size_t index) const;
        Value operator[](const char* key) const;
        Value operator[](const std::string& key) const;

        operator void*() const;
        operator bool() const;
        operator int64_t() const;
        operator double() const;
        operator std::string() const;

    friend std::ostream& operator<<(std::ostream&, const Value&);
};


} /* namespace je */

#endif /* JEPP_VALUE_H_ */
