#ifndef JE_TOKEN_H_
#define JE_TOKEN_H_

#include <inttypes.h>
#include "je_parse.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* CONSTANTS
*/

#define JE_DECOR_TF 0x01


/* ***************************************************************************
* TYPES
*/

typedef struct JE_TOKEN {
    char* text;
    size_t first_pos;
    size_t first_line;
    size_t first_column;
    size_t last_pos;
    size_t last_line;
    size_t last_column;

    struct JE_TOKEN* prev;
    struct JE_TOKEN* next;
    int flag;
} JE_TOKEN;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_TOKEN* je_newtoken(size_t first_pos, size_t first_line, size_t first_column, JE_TOKEN* prev);
void je_unlinktoken(JE_TOKEN* token);
void je_freetoken(JE_TOKEN* token, int recursive);

char* je_astrtoken(JE_TOKEN* begin, JE_TOKEN* end);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_TOKEN_H_ */
