#ifndef JE_VEC_H_
#define JE_VEC_H_

#include <inttypes.h>


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_VAL JE_VAL;


/* ***************************************************************************
* TYPES
*/

typedef struct JE_VEC {
    JE_VAL** item;
    size_t capacity;
    size_t length;
    size_t count;
} JE_VEC;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_VEC* je_newvec();
JE_VEC* je_dupvec(JE_VEC* vec);
void je_freevec(JE_VEC* vec);

int je_vecset(JE_VEC* vec, size_t index, JE_VAL* item);
void je_vecpop(JE_VEC* vec);
void je_vecpush(JE_VEC* vec, JE_VAL* item);
JE_VAL* je_vecget(JE_VEC* vec, size_t index);
JE_VAL* je_vecback(JE_VEC* vec);

int je_veccmp(JE_VEC* vec1, JE_VEC* vec2);
char* je_vecastr(JE_VEC* vec);


#endif /* JE_VEC_H_ */
