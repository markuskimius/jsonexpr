#ifndef JE_NOD_H_
#define JE_NOD_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATION
*/

typedef struct JE_ASTINFO JE_ASTINFO;
typedef struct JE_NODE JE_NODE;


/* ***************************************************************************
* TYPES
*/

typedef struct JE_NOD {
    const JE_NODE*    node;
    const JE_ASTINFO* info;
} JE_NOD;


/* ***************************************************************************
* FUNCTIONS
*/

JE_NOD JE_NodCreate(const JE_NODE* node, const JE_ASTINFO* info);
void JE_NodDestroy(JE_NOD* nod);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_NOD_H_ */
