#ifndef NVECTOR_H_
#define NVECTOR_H_

#include <inttypes.h>


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct AST_NODE AST_NODE;


/* ***************************************************************************
* TYPES
*/

typedef struct NVEC {
    AST_NODE** item;
    size_t length;
    size_t capacity;
} NVEC;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

NVEC* newnvec();
void freenvec(NVEC* vec);
void pushnvec(NVEC* vec, AST_NODE* item);
void setnvec(NVEC* vec, size_t index, AST_NODE* item);
void popnvec(NVEC* vec);
AST_NODE* getnvec(NVEC* vec, size_t index);
AST_NODE* backnvec(NVEC* vec);


#endif /* NVECTOR_H_ */
