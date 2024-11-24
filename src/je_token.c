#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "je_token.h"
#include "je_util.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_TOKEN* je_newtoken(size_t pos, size_t line, size_t column, JE_TOKEN* prev) {
    JE_TOKEN* token = calloc(1, sizeof(JE_TOKEN));

    token->first_pos = pos;
    token->first_line = line;
    token->first_column = column;
    token->last_pos = pos;
    token->last_line = line;
    token->last_column = column;
    token->prev = prev;
    token->next = prev ? token->prev->next : NULL;

    if(token->prev) token->prev->next = token;
    if(token->next) token->next->prev = token;

    return token;
}


void je_unlinktoken(JE_TOKEN* token) {
    if(token->prev) token->prev->next = token->next;
    if(token->next) token->next->prev = token->prev;

    token->prev = NULL;
    token->next = NULL;
}


void je_freetoken(JE_TOKEN* token, int recursive) {
    if(token->next && recursive) je_freetoken(token->next, recursive);
    if(token->text) free(token->text);

    token->text = NULL;
    token->prev = NULL;
    token->next = NULL;

    free(token);
}


char* je_astrtoken(JE_TOKEN* begin, JE_TOKEN* end) {
    JE_TOKEN* next = begin;
    char* buf = NULL;

    while(next && next != end) {
        buf = je_astrcat(buf, next->text);
        next = next->next;
    }

    return buf;
}
