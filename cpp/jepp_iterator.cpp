#include <cstdlib>
#include "je/je.h"
#include "je/je_str.h"
#include "jepp_error.h"
#include "jepp_iterator.h"
#include "jepp_value.h"

namespace je {


/* ***************************************************************************
* METHODS
*/

Iterator::Iterator(const JE_VALUE& wvalue, size_t index) : index(index) {
    this->wvalue = (JE_VALUE*) malloc(sizeof(JE_VALUE));
    *this->wvalue = JE_VCALL(&wvalue,clone);
}

Iterator::~Iterator() {
    JE_VCALL(this->wvalue,destroy);
    free(this->wvalue);
}

std::string Iterator::key() const {
    JE_STR* key = NULL;

    if(this->wvalue->type == &JE_typeMap && this->index < this->wvalue->data.map->size) {
        key = &this->wvalue->data.map->key[this->index];
    }

    if(!key) {
        throw TypeError(std::string("Cannot get the key of ") + JE_VCALL(this->wvalue,getTypeCstr));
    }

    return std::string((const char*) key->data);
}

Value Iterator::value() const {
    JE_VALUE* value = NULL;

    if(this->wvalue->type == &JE_typeMap && this->index < this->wvalue->data.map->size) {
        value = &this->wvalue->data.map->value[this->index];
    }
    else if(this->wvalue->type == &JE_typeVec && this->index < this->wvalue->data.vec->length) {
        value = &this->wvalue->data.vec->data[this->index];
    }

    if(!value) {
        throw TypeError(std::string("Cannot get the value of ") + JE_VCALL(this->wvalue,getTypeCstr));
    }

    return Value(value);
}

Value Iterator::operator*() const {
    return this->value();
}

Iterator& Iterator::operator++() {
    this->index++;

    return *this;
}

Iterator& Iterator::operator++(int) {
    this->index++;

    return *this;
}

bool Iterator::operator==(const Iterator& other) const {
    return this->wvalue->type == other.wvalue->type && this->wvalue->data.map == other.wvalue->data.map && this->index == other.index;
}

bool Iterator::operator!=(const Iterator& other) const {
    return this->wvalue->type != other.wvalue->type || this->wvalue->data.map != other.wvalue->data.map || this->index != other.index;
}


} /* namespace je */
