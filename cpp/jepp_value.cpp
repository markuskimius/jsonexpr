#include <cstdlib>
#include <cstring>
#include <iostream>
#include "je/je.h"
#include "je/je_str.h"
#include "je/je_util.h"
#include "jepp_error.h"
#include "jepp_iterator.h"
#include "jepp_value.h"

namespace je {


/* ***************************************************************************
* METHODS
*/

Value::Value() : isref(false) {
    this->wvalue = (JE_VALUE*) malloc(sizeof(JE_VALUE));
    *this->wvalue = JE_ValueNul();
}

Value::Value(bool tru) : isref(false) {
    this->wvalue = (JE_VALUE*) malloc(sizeof(JE_VALUE));
    *this->wvalue = JE_ValueTru(tru);
}

Value::Value(int64_t i64) : isref(false) {
    this->wvalue = (JE_VALUE*) malloc(sizeof(JE_VALUE));
    *this->wvalue = JE_ValueI64(i64);
}

Value::Value(double f64) : isref(false) {
    this->wvalue = (JE_VALUE*) malloc(sizeof(JE_VALUE));
    *this->wvalue = JE_ValueF64(f64);
}

Value::Value(const char* cstr) : isref(false) {
    this->wvalue = (JE_VALUE*) malloc(sizeof(JE_VALUE));
    *this->wvalue = JE_ValueStr(JE_StrCreate(strdup(cstr)));
}

Value::Value(const std::string& str) : Value(str.c_str()) {
}

Value::Value(const JE_VALUE& wvalue) : isref(false) {
    this->wvalue = (JE_VALUE*) malloc(sizeof(JE_VALUE));
    *this->wvalue = JE_VCALL(&wvalue,clone);
}

Value::Value(JE_VALUE* wvalue) : wvalue(wvalue), isref(true) {
}

Value::Value(const Value& value) : isref(false) {
    this->wvalue = (JE_VALUE*) malloc(sizeof(JE_VALUE));
    *this->wvalue = JE_VCALL(value.wvalue,clone);
}

Value::Value(Value&& value) : isref(value.isref) {
    this->wvalue = (JE_VALUE*) malloc(sizeof(JE_VALUE));
    *this->wvalue = *value.wvalue;

    *value.wvalue = JE_ValueNul();
    value.isref = false;
}

Value::~Value() {
    if(!this->isref) {
        JE_VCALL(this->wvalue,destroy);
        free(this->wvalue);
    }
}

size_t Value::size() const {
    JE_VALUE length = JE_VCALL(this->wvalue,length);

    if(length.type != &JE_typeI64) {
        throw TypeError(std::string("Cannot get the size of type ") + JE_VCALL(this->wvalue,getTypeCstr));
    }

    return length.data.i64;
}

Iterator Value::begin() {
    if(this->wvalue->type != &JE_typeMap && this->wvalue->type != &JE_typeVec) {
        throw TypeError(std::string("Cannot iterate ") + JE_VCALL(this->wvalue,getTypeCstr));
    }

    return Iterator(*this->wvalue, 0);
}

Iterator Value::end() {
    JE_VALUE length = JE_VCALL(this->wvalue,length);

    if(this->wvalue->type != &JE_typeMap && this->wvalue->type != &JE_typeVec) {
        throw TypeError(std::string("Cannot iterate ") + JE_VCALL(this->wvalue,getTypeCstr));
    }

    return Iterator(*this->wvalue, length.data.i64);
}

Value& Value::operator=(const Value& value) {
    JE_VCALL(this->wvalue,destroy);

    *this->wvalue = JE_VCALL(value.wvalue,clone);

    return *this;
}

Value Value::operator[](size_t index) const {
    JE_VEC* wvec = this->wvalue->data.vec;

    /* Validation */
    if(this->wvalue->type != &JE_typeVec) throw TypeError(std::string("ARRAY expected, got ") + JE_VCALL(this->wvalue,getTypeCstr));
    if(index >= wvec->length) throw IndexError(std::string("Index must be < ") + std::to_string(wvec->length) + ", got " + std::to_string(index));

    return Value(JE_VecGet(*wvec,index));
}

Value Value::operator[](const char* key) const {
    JE_STR wkey = JE_StrCreate((char*) key);  /* Steal c_str() */
    JE_MAP* wmap = this->wvalue->data.map;
    JE_VALUE* result = nullptr;

    /* Validation */
    if(this->wvalue->type != &JE_typeMap) throw TypeError(std::string("OBJECT expected, got ") + JE_VCALL(this->wvalue,getTypeCstr));

    result = JE_MapGet(*wmap, wkey);
    if(!result) {
        result = JE_MapSet(wmap, JE_StrClone(wkey), JE_ValueNul());
    }

    /* Do not free wkey since it was created with a stolen char* */

    return Value(result);
}

Value Value::operator[](const std::string& key) const {
    return this->operator[](key.c_str());
}

Value::operator void*() const {
    return nullptr;
}

Value::operator bool() const {
    return JE_VCALL(this->wvalue,toTru);
}

Value::operator int64_t() const {
    if(this->wvalue->type != &JE_typeI64) {
        throw UsageError(std::string("Invalid type. INT64 expected, got ") + JE_VCALL(this->wvalue,getTypeCstr));
    }

    return this->wvalue->data.i64;
}

Value::operator double() const {
    if(this->wvalue->type != &JE_typeF64) {
        throw UsageError(std::string("Invalid type. FLOAT64 expected, got ") + JE_VCALL(this->wvalue,getTypeCstr));
    }

    return this->wvalue->data.f64;
}

Value::operator std::string() const {
    char* cstr = JE_VCALL(this->wvalue,toCstr);
    std::string str = std::string(cstr);

    free(cstr);

    return str;
}

/* ***************************************************************************
* FRIENDS
*/

std::ostream& operator<<(std::ostream& os, const Value& value) {
    char* cstr = JE_VCALL(value.wvalue,toCstr);

    os << cstr;
    free(cstr);

    return os;
}


} /* namespace je */
