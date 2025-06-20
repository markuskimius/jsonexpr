#include <string.h>
#include "je_nod.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_NOD JE_NodCreate(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_NOD nod;

    nod.node = node;
    nod.info = info;

    return nod;
}

void JE_NodDestroy(JE_NOD* nod) {
    memset(nod, 0, sizeof(JE_NOD));
}
