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
    size_t capacity;
    size_t length;
    size_t count;
} VEC;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VEC* newvec();
VEC* dupvec(VEC* vec);
void freevec(VEC* vec);
char* astrvec(VEC* vec);
void pushvec(VEC* vec, VALUE* item);
int setvec(VEC* vec, size_t index, VALUE* item);
void popvec(VEC* vec);
VALUE* getvec(VEC* vec, size_t index);
VALUE* backvec(VEC* vec);

int cmpvec(VEC* vec1, VEC* vec2);


#endif /* VECTOR_H_ */
