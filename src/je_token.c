#include <stdlib.h>
#include <string.h>
#include "je_token.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_TOKEN JE_TokenCreate(size_t pos, size_t line, size_t column) {
    JE_TOKEN token;

    token.text = JE_StrCreate(NULL);
    token.first_pos = token.last_pos = pos;
    token.first_line = token.last_line = line;
    token.first_column = token.last_column = column;

    return token;
}

void JE_TokenDestroy(JE_TOKEN* token) {
    memset(token, 0, sizeof(JE_TOKEN));
}

/**
* Convert a token range [begin, end) to a JE_STR.  The returned string must be
* destroyed.
*/
JE_STR JE_TokenRangeToStr(const JE_TOKEN* begin, const JE_TOKEN* end) {
    JE_STR str = JE_StrCreate(NULL);
    const JE_TOKEN* iter = begin;

    while(iter != end) {
        JE_StrPushStr(&str, iter->text);
        iter++;
    }

    return str;
}

/**
* Convert a token range [begin, end) to a C string.  The returned C string must
* be freed.
*/
char* JE_TokenRangeToCstr(const JE_TOKEN* begin, const JE_TOKEN* end) {
    JE_STR str = JE_TokenRangeToStr(begin, end);

    return (char*) str.data;
}

/**
* Convert a token range [begin, end) to a quoted C string.  The returned C
* string must be freed.
*/
char* JE_TokenRangeToQstr(const JE_TOKEN* begin, const JE_TOKEN* end) {
    JE_STR str = JE_TokenRangeToStr(begin, end);
    char* qstr = JE_StrToQstr(str);

    JE_StrDestroy(&str);

    return qstr;
}
