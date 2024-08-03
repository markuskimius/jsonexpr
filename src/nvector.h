#ifndef NVECTOR_H_
#define NVECTOR_H_

#include <inttypes.h>


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct NODE NODE;


/* ***************************************************************************
* TYPES
*/

typedef struct NVEC {
    NODE** item;
    size_t length;
    size_t capacity;
} NVEC;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

NVEC* newnvec();
void freenvec(NVEC* vec);
void pushnvec(NVEC* vec, NODE* item);
void setnvec(NVEC* vec, size_t index, NODE* item);
void popnvec(NVEC* vec);
NODE* getnvec(NVEC* vec, size_t index);
NODE* backnvec(NVEC* vec);


#endif /* NVECTOR_H_ */
