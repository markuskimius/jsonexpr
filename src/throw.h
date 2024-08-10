#ifndef EXCEPTION_H_
#define EXCEPTION_H_


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct NODE NODE;
typedef struct TOKEN TOKEN;


/* ***************************************************************************
* MACROS
*/

#define die(format, args...) _die(__FILE__, __FUNCTION__, __LINE__, format, args)


/* ***************************************************************************
* GLOBALS
*/

extern char* error_text;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

void _die(const char* file, const char* func, size_t line, const char* format, ...);
void raise(const char* format, ...);
void thrownode(NODE* node, const char* format, ...);
void throwtoken(TOKEN* token, const char* format, ...);


#endif /* EXCEPTION_H_ */
