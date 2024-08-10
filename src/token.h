#ifndef TOKEN_H_
#define TOKEN_H_

#include <inttypes.h>
#include "parse.h"


/* ***************************************************************************
* TYPES
*/

typedef struct TOKEN {
    int type;
    char* text;
    YYLTYPE loc;

    union {
        int64_t i;
        double f;
        char* s;
    } value;
} TOKEN;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

TOKEN* newtoken(int type, const YYLTYPE* loc, const char* text);
TOKEN* inttoken(int type, const YYLTYPE* loc, const char* text, int64_t i);
TOKEN* dbltoken(int type, const YYLTYPE* loc, const char* text, double f);
TOKEN* strtoken(int type, const YYLTYPE* loc, const char* text, const char* s);
TOKEN* astrtoken(int type, const YYLTYPE* start, const YYLTYPE* end, char* text, char* s);
void freetoken(TOKEN* token);


#endif /* TOKEN_H_ */
