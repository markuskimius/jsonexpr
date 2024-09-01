#ifndef VEC_H_
#define VEC_H_

#include <inttypes.h>


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct VAL VAL;


/* ***************************************************************************
* TYPES
*/

typedef struct VEC {
    VAL** item;
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

int vecset(VEC* vec, size_t index, VAL* item);
void vecpop(VEC* vec);
void vecpush(VEC* vec, VAL* item);
VAL* vecget(VEC* vec, size_t index);
VAL* vecback(VEC* vec);

int veccmp(VEC* vec1, VEC* vec2);
char* vecastr(VEC* vec);


#endif /* VEC_H_ */
