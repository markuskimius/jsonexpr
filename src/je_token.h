#ifndef JE_TOKEN_H_
#define JE_TOKEN_H_

#include <inttypes.h>
#include "je_parse.h"
#include "je_str.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATION
*/

typedef struct JE_AST JE_AST;


/* ***************************************************************************
* TYPES
*/

typedef struct JE_TOKEN {
    JE_STR text;
    size_t first_pos;
    size_t first_line;
    size_t first_column;
    size_t last_pos;
    size_t last_line;
    size_t last_column;
} JE_TOKEN;


/* ***************************************************************************
* FUNCTIONS
*/

JE_TOKEN JE_TokenCreate(size_t pos, size_t line, size_t column);
void JE_TokenDestroy(JE_TOKEN* token);

JE_STR JE_TokenRangeToStr(const JE_TOKEN* begin, const JE_TOKEN* end);
char* JE_TokenRangeToCstr(const JE_TOKEN* begin, const JE_TOKEN* end);
char* JE_TokenRangeToQstr(const JE_TOKEN* begin, const JE_TOKEN* end);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_TOKEN_H_ */
