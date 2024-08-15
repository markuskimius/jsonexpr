#ifndef THROW_H_
#define THROW_H_


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct NODE NODE;
typedef struct YYLTYPE YYLTYPE;


/* ***************************************************************************
* MACROS
*/

#define die(args...) _die(__FILE__, __FUNCTION__, __LINE__, args)


/* ***************************************************************************
* GLOBALS
*/

extern char* error_text;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

void _die(const char* file, const char* func, size_t line, const char* format, ...);
void raise(const char* format, ...);
void throw(YYLTYPE* loc, const char* format, ...);


#endif /* THROW_H_ */
