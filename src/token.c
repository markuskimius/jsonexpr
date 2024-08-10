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

TOKEN* newtoken(int type, const YYLTYPE* loc, const char* text) {
    TOKEN* token = calloc(1, sizeof(TOKEN));

    token->type = type;
    token->text = text ? strdup(text) : NULL;
    token->loc = *loc;

    return token;
}

TOKEN* inttoken(int type, const YYLTYPE* loc, const char* text, int64_t i) {
    TOKEN* token = newtoken(type, loc, text);

    token->value.i = i;

    return token;
}

TOKEN* dbltoken(int type, const YYLTYPE* loc, const char* text, double f) {
    TOKEN* token = newtoken(type, loc, text);

    token->value.f = f;

    return token;
}

TOKEN* strtoken(int type, const YYLTYPE* loc, const char* text, const char* s) {
    TOKEN* token = newtoken(type, loc, text);

    token->value.s = s ? strdup(s) : calloc(1, SMINSIZE);

    return token;
}

TOKEN* astrtoken(int type, const YYLTYPE* start, const YYLTYPE* end, char* text, char* s) {
    TOKEN* token = newtoken(type, start, NULL);

    token->text = text;
    token->value.s = s;
    token->loc.last_line = end->last_line;
    token->loc.last_column = end->last_column;

    return token;
}

void freetok(TOKEN* token) {
    switch(token->type) {
        case IDENT_T:
        case STRING_T:
            free(token->value.s);
            break;
    }

    free(token->text);
    free(token);
}
