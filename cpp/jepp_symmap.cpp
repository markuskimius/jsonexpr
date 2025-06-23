#include "je/je.h"
#include "jepp_error.h"
#include "jepp_symmap.h"

namespace je {


/* ***************************************************************************
* METHODS
*/

Symmap::Symmap() {
    this->wsymmap = JE_SymmapCreate(JE_SYMBOLS_DEFAULT);
}

Symmap::Symmap(const std::string& symbols) {
    /*
    * Vanilla C++ doesn't have a native JSON parser.  Thankfully, JSONexpr
    * itself is a kind of a JSON parser so we just evaluate `symbols` to get a
    * map object.
    */
    JE_AST ast = JE_Parse(symbols.c_str());
    JE_MAP* tmpmap = JE_SymmapCreate(0);
    JE_VALUE result = JE_AstEval(ast, tmpmap);
    JE_MAP* resmap = result.data.map;

    /* Validation */
    if(result.type != &JE_typeMap) {
        throw UsageError("Invalid symbol definition");
    }

    this->wsymmap = JE_SymmapCreate(JE_SYMBOLS_DEFAULT);

    /* Move all items */
    for(size_t i=0; i<resmap->size; i++) {
        JE_STR* key = &resmap->key[i];
        JE_VALUE* value = &resmap->value[i];

        JE_MapSet(this->wsymmap, *key, *value);
    }

    /* Flag every item as moved */
    resmap->size = 0;

    JE_VCALL(&result,destroy);
    JE_SymmapDestroy(tmpmap);
    JE_AstDestroy(&ast);
}

Symmap::~Symmap() {
    JE_SymmapDestroy(this->wsymmap);
}

Value Symmap::operator[](const char* key) const {
    JE_STR wkey = JE_StrCreate((char*) key);  /* Steal c_str() */
    JE_VALUE* result = nullptr;

    result = JE_SymmapGet(*this->wsymmap, wkey);
    if(!result) {
        result = JE_SymmapSet(this->wsymmap, JE_StrClone(wkey), JE_ValueNul(), 1);
    }

    /* Do not free wkey since it was created with a stolen char* */

    return Value(result);
}

Value Symmap::operator[](const std::string& key) const {
    return this->operator[](key.c_str());
}


} /* namespace je */
