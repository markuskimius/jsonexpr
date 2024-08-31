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
#define LogicError(args...) throw("LogicError", args)
#define ParseError(args...) throw("ParseError", args)
#define RuntimeError(args...) throw("RuntimeError", args)


/* ***************************************************************************
* GLOBALS
*/

extern char* throwText;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

void _die(const char* file, const char* func, size_t line, const char* format, ...);
void throw(const char* type, YYLTYPE* loc, const char* format, ...);
void throwLater(const char* format, ...);


#endif /* THROW_H_ */
