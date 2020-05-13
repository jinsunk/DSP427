#include <stdlib.h>
#include <string.h>		// memcpy
#include "queue.h"
#include "console.h"

int32_t InitQ(PQUEUE pQ, int32_t QnNode /* number of Q nodes */, int32_t QpSize /* byte size */)
{
	PNODE pNewNode;
	PQTAG pNewTag;
	int32_t i;

	// Pool SIze를 정한다
	pQ->pPoolBase = malloc(QpSize);
	if ( pQ->pPoolBase == NULL )	return -1;

	pQ->pPoolCpt = pQ->pPoolBase;
	pQ->pPoolEnd = pQ->pPoolBase + QpSize;
	pQ->FreeSize = QpSize;

	ListInit(&pQ->lUsedQNode, ASCENT | LINEAR);
	ListInit(&pQ->lFreeQNode, ASCENT | LINEAR);

	for (i = 0; i < QnNode; i++ )
	{
		// Node를 생성한다
		pNewNode = (PNODE)malloc(sizeof(NODE));

		if ( pNewNode )
		{
			// 생성된 Node에 변수를 생선하여 연결한다
			pNewTag = (PQTAG)malloc(sizeof(QTAG));
			if ( pNewTag )
			{
				pNewNode->pObject = pNewTag;

				// free list에 연결한다
				ListAddNodeTail(&pQ->lFreeQNode, pNewNode);
			}
			else
			{
#ifdef	_CONSOLE_H_
				console_printf("Queue: halt! not enough memory for tag\r\n");
#endif
				while(1);
			}
		}
		else
		{
#ifdef	_CONSOLE_H_
			console_printf("Queue: halt! not enough memory for node\r\n");
#endif
			while(1);
		}
	}

	return 0;
}

int32_t GetFreeSizeQ(PQUEUE pQ)
{
	// free node가 있는지 검사한다
	if ( pQ->lFreeQNode.m_nCount > 0 )
	{
		return pQ->FreeSize;
	}
	else return 0;
}

int32_t InsertQ(PQUEUE pQ, PQTAG pNewTag, int32_t priority)
{
	uint8_t *p8Data;
	uint32_t u32Size;
	PNODE pNode;
	PQTAG pTag;

	if ( pQ->FreeSize < pNewTag->size )
		return -1;	// not enough pool

	if (pQ->lFreeQNode.m_nCount == 0)
		return -1; // not enough node

	// lFreeNode의 head를 때어낸다
	pNode = ListRemoveHeadNode(&pQ->lFreeQNode);
	pTag  = (PQTAG)pNode->pObject;

	// QTag를 기록한다
	pTag->msg = pNewTag->msg;
	pTag->params = pNewTag->params;
	pTag->size = pNewTag->size;
	pTag->head = pQ->pPoolCpt;	// 현재 point를 기록

	// data를 기록한다
	p8Data = (uint8_t*)pNewTag->head;
	u32Size = (uint32_t)pNewTag->size;

	while(u32Size--)
	{
		*pQ->pPoolCpt++ = *p8Data++;
		if ( pQ->pPoolCpt == pQ->pPoolEnd )
			pQ->pPoolCpt = pQ->pPoolBase;

		pQ->FreeSize--;
	}

	// 생성한 노드를 UsedList에 더한다.
	if ( priority == 0 )
		ListAddNodeHead(&pQ->lUsedQNode, pNode);
	else
		ListAddNodeTail(&pQ->lUsedQNode, pNode);

//	pTag->tail = pQ->pPoolCpt;

	return 1;
}

int32_t WithdrawalQ(PQUEUE pQ, PQTAG pNewTag, void *NewData)
{
	if (pQ->lUsedQNode.m_nCount)
	{
		PNODE pNode;
		PQTAG pTag;
		uint32_t u32Dsize;

		uint8_t *ptr_pool;
		uint8_t *data;

		// 노드에서 UsedList에서 제거한다.
		pNode = ListRemoveHeadNode(&pQ->lUsedQNode);

		pTag = (PQTAG)pNode->pObject;

		memcpy(pNewTag, pTag, sizeof(QTAG));

		u32Dsize = pNewTag->size;
		ptr_pool = pNewTag->head;
		data = (uint8_t*)NewData;

		while ( u32Dsize-- )
		{
			*data++ = *ptr_pool++;

		 	if ( ptr_pool == pQ->pPoolEnd )
		 		ptr_pool = pQ->pPoolBase;

			pQ->FreeSize++;
		}

		// 제거한 노드를 FreeList에 더한다.
		ListAddNodeTail(&pQ->lFreeQNode, pNode);

		return (int32_t)1;		// 값이 있으면 0이 아닌 값을 return 한다
	}

	return (int32_t)0;
}

int32_t FlushQ( PQUEUE pQ )
{
	PNODE pNode;

	while (pQ->lUsedQNode.m_nCount)
	{
		// 노드에서 UsedList에서 제거한다.
		pNode = ListRemoveHeadNode(&pQ->lUsedQNode);

		// 제거한 노드를 FreeList에 더한다.
		ListAddNodeTail(&pQ->lFreeQNode, pNode);
	}

	return 0;
}

int32_t DistroyQ(PQUEUE pQ)
{
	PNODE pNode;
	PQTAG pTag;

	// 사용중인 list을 모두 비운다
	while (pQ->lUsedQNode.m_nCount)
	{
		pNode = ListRemoveHeadNode(&pQ->lUsedQNode);
		pTag = (PQTAG)pNode->pObject;

		free(pTag);
		free(pNode);
	}

	while (pQ->lFreeQNode.m_nCount)
	{
		pNode = ListRemoveHeadNode(&pQ->lFreeQNode);
		pTag = (PQTAG)pNode->pObject;

		free(pTag);
		free(pNode);
	}

	pQ->FreeSize = (int32_t)(pQ->pPoolEnd - pQ->pPoolBase);

	return 0;
}



