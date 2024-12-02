#ifndef JE_NAV_H_
#define JE_NAV_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_NODE JE_NODE;
typedef struct JE_NAVNODE JE_NAVNODE;


/* ***************************************************************************
* TYPES
*/

typedef struct JE_NAV {
    struct JE_NAVNODE* root;
    struct JE_NAVNODE* curr;
} JE_NAV;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_NAV* je_newnav(JE_NODE* node);
void je_freenav(JE_NAV* nav);

int je_naventer(JE_NAV* nav);
int je_navexit(JE_NAV* nav);
int je_navprev(JE_NAV* nav);
int je_navnext(JE_NAV* nav);

const char* je_navtype(JE_NAV* nav);
JE_NODE* je_navnode(JE_NAV* nav);
JE_NODE* je_navkeynode(JE_NAV* nav);
JE_NODE* je_navvaluenode(JE_NAV* nav);

const char* je_navtextat(JE_NAV* nav);
const char* je_navtextafter(JE_NAV* nav);
const char* je_navtextbefore(JE_NAV* nav);

char* je_navnodetree(JE_NAVNODE* navnode);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_NAV_H_ */
