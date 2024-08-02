#ifndef VECTOR_H_
#define VECTOR_H_

#include <inttypes.h>


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct VALUE VALUE;


/* ***************************************************************************
* TYPES
*/

typedef struct VEC {
    VALUE** item;
    size_t length;
    size_t capacity;
} VEC;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VEC* newvec();
void freevec(VEC* vec);
char* astrvec(VEC* vec);
void pushvec(VEC* vec, VALUE* item);
void setvec(VEC* vec, size_t index, VALUE* item);
void popvec(VEC* vec);
VALUE* getvec(VEC* vec, size_t index);
VALUE* backvec(VEC* vec);


#endif /* VECTOR_H_ */
