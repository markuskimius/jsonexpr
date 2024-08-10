#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "parse.h"


/* ***************************************************************************
* CONSTANTS
*/

#define SMINSIZE 8


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

TOKEN* newtoken(int type, const YYLTYPE* loc) {
    TOKEN* token = calloc(1, sizeof(TOKEN));

    token->type = type;
    token->loc = *loc;

    return token;
}

TOKEN* inttoken(int type, const YYLTYPE* loc, int64_t i) {
    TOKEN* token = newtoken(type, loc);

    token->value.i = i;

    return token;
}

TOKEN* dbltoken(int type, const YYLTYPE* loc, double f) {
    TOKEN* token = newtoken(type, loc);

    token->value.f = f;

    return token;
}

TOKEN* strtoken(int type, const YYLTYPE* loc, const char* s) {
    TOKEN* token = newtoken(type, loc);

    token->value.s = s ? strdup(s) : calloc(1, SMINSIZE);

    return token;
}

TOKEN* astrtoken(int type, const YYLTYPE* start, const YYLTYPE* end, char* s) {
    TOKEN* token = newtoken(type, start);

    token->value.s = s;
    token->loc.last_pos = end->last_pos;
    token->loc.last_line = end->last_line;
    token->loc.last_column = end->last_column;

    return token;
}

const char* tokentext(TOKEN* token) {
    if(!token->text) {
        YYLTYPE* loc = &token->loc;
        size_t length = loc->last_pos - loc->first_pos + 1;
        char* code = *loc->code;

        token->text = calloc(1, length);
        snprintf(token->text, length, "%s", code + loc->first_pos);
    }

    return token->text;
}

void freetoken(TOKEN* token) {
    switch(token->type) {
        case IDENT_T:
        case STRING_T:
            free(token->value.s);
            break;
    }

    if(token->text) free(token->text);
    free(token);
}
