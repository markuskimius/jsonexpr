#ifndef JE_THROW_H_
#define JE_THROW_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_AST JE_AST;
typedef struct JE_TOKEN JE_TOKEN;


/* ***************************************************************************
* MACROS
*/

#define JE_SyntaxError(args...) JE_Error("JE_SyntaxError", args)
#define JE_RuntimeError(args...) JE_Error("JE_RuntimeError", args)
#define JE_InternalError(args...) JE_Error("JE_InternalError", args)


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

void JE_Error(const char* type, const JE_TOKEN* begin, const JE_TOKEN* end, const char* format, ...);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_THROW_H_ */
