#ifndef TOKEN_H_
#define TOKEN_H_

#include <inttypes.h>
#include "parse.h"


/* ***************************************************************************
* TYPES
*/

typedef struct TOKEN {
    int type;
    YYLTYPE loc;
    char* text;

    union {
        int64_t i;
        double f;
        char* s;
    } value;
} TOKEN;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

TOKEN* newtoken(int type, const YYLTYPE* loc);
TOKEN* inttoken(int type, const YYLTYPE* loc, int64_t i);
TOKEN* dbltoken(int type, const YYLTYPE* loc, double f);
TOKEN* strtoken(int type, const YYLTYPE* loc, const char* s);
TOKEN* astrtoken(int type, const YYLTYPE* start, const YYLTYPE* end, char* s);
const char* tokentext(TOKEN* token);
void freetoken(TOKEN* token);


#endif /* TOKEN_H_ */
