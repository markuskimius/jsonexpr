#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "je_token.h"
#include "je_util.h"


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static void _tokenswap(JE_TOKEN* token1, JE_TOKEN* token2) {
    JE_TOKEN tmp;

    memcpy(&tmp, token1, sizeof(JE_TOKEN));
    memcpy(token1, token2, sizeof(JE_TOKEN));
    memcpy(token2, &tmp, sizeof(JE_TOKEN));
}

static void _tokenrepos(JE_TOKEN* token, size_t first_pos, size_t first_line, size_t first_column) {
    ssize_t delta_pos = token->last_pos - token->first_pos;
    ssize_t delta_line = token->last_line - token->first_line;
    ssize_t delta_column = token->last_column - token->first_column;

    token->first_pos = first_pos;
    token->first_line = first_line;
    token->first_column = first_column;

    token->last_pos = first_pos + delta_pos;
    token->last_line = first_line + delta_line;
    if(delta_line == 0) token->last_column = first_column + delta_column;

    if(token->next) _tokenrepos(token->next, token->last_pos, token->last_line, token->last_column);
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_TOKEN* JE_TokenNew(size_t pos, size_t line, size_t column, JE_TOKEN* prev) {
    JE_TOKEN* token = JE_Calloc(1, sizeof(JE_TOKEN));

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

void JE_TokenDelete(JE_TOKEN* token, int recursive) {
    if(token) {
        if(token->next && recursive) JE_TokenDelete(token->next, recursive);
        if(token->text) free(token->text);

        token->text = NULL;
        token->prev = NULL;
        token->next = NULL;

        free(token);
    }
}

JE_TOKEN* JE_TokenFindHead(JE_TOKEN* token) {
    while(token && token->prev) token = token->prev;

    return token;
}

JE_TOKEN* JE_TokenFindTail(JE_TOKEN* token) {
    while(token && token->next) token = token->next;

    return token;
}

JE_TOKEN* JE_TokenDetach(JE_TOKEN* begin, JE_TOKEN* end) {
    JE_TOKEN* prev = begin->prev;
    JE_TOKEN* next = end->next;

    /* Detach token */
    if(begin->prev) begin->prev->next = end->next;
    if(end->next) end->next->prev = begin->prev;

    begin->prev = NULL;
    end->next = NULL;

    /* Update position data */
    if(prev && next) _tokenrepos(next, prev->last_pos, prev->last_line, prev->last_column);
    else if(next) _tokenrepos(next, 0, 1, 1);
    _tokenrepos(begin, 0, 1, 1);

    return begin;
}

JE_TOKEN* JE_TokenAttachAt(JE_TOKEN* dest, JE_TOKEN* src) {
    JE_TOKEN* dprev = dest->prev;
    JE_TOKEN* srcend = JE_TokenFindTail(src);

    assert(src->prev == NULL);
    assert(srcend->next == NULL);

    /* Attach src at dest */
    src->prev = dprev;
    srcend->next = dest;
    if(dprev) dprev->next = src;
    dest->prev = srcend;

    /* Update position data */
    _tokenrepos(src, dest->last_pos, dest->last_line, dest->last_column);

    return src;
}

JE_TOKEN* JE_TokenAttachTo(JE_TOKEN* dest, JE_TOKEN* src) {
    JE_TOKEN* dest2 = dest->next;
    JE_TOKEN* srcend = JE_TokenFindTail(src);

    assert(src->prev == NULL);
    assert(srcend->next == NULL);

    /* Break dest into 2 parts */
    dest->next = NULL;
    dest2->prev = NULL;

    /* Insert src */
    dest->next = src;
    src->prev = dest;
    srcend->next = dest2;
    dest2->prev = srcend;

    /* Update position data */
    _tokenrepos(src, dest->last_pos, dest->last_line, dest->last_column);

    return dest;
}

char* JE_TokenToAstr(JE_TOKEN* begin, JE_TOKEN* end) {
    JE_TOKEN* next = begin;
    char* buf = NULL;

    while(next && next != end) {
        buf = JE_AstrCat(buf, next->text);
        next = next->next;
    }

    return buf;
}
