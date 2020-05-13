#ifndef	_QUEUE_H_
#define	_QUEUE_H_

	#ifndef _STDINT
	#include <stdint.h>
	#endif

	#ifndef	_LIST_H_
	#include "list.h"
	#endif

typedef	struct {
	LIST lUsedQNode;
	LIST lFreeQNode;
	PNODE pNodeVal;

	uint8_t *pPoolBase;
	uint8_t *pPoolEnd;
	uint8_t *pPoolCpt;

	int32_t FreeSize;
} QUEUE, *PQUEUE;

typedef struct {
	uint32_t msg;
	uint32_t params;
	uint32_t size;

	uint8_t *head;
//	uint8_t* tail;
} QTAG, *PQTAG;

#ifdef __cplusplus
extern "C" {
#endif

extern int32_t InitQ(PQUEUE pQ, int32_t QnNode /* number of Q nodes */, int32_t QpSize /* byte size */);

//extern int32_t InsertQ(PQUEUE pQ, uint32_t msg, uint32_t params, uint32_t size, uint8_t *data, int32_t priority);
extern int32_t InsertQ(PQUEUE pQ, PQTAG pNewTag, int32_t priority);
extern int32_t WithdrawalQ(PQUEUE pQ, PQTAG pNewTag, void *NewData);
extern int32_t FlushQ(PQUEUE pQ);
extern int32_t DistroyQ(PQUEUE pQ);

#ifdef __cplusplus
}
#endif




#endif
