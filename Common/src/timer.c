#include "timer.h"
#include "board.h"

#ifdef DEBUG
#include <stdio.h>		// for print debug message
#endif



#define	MAX_TIMER	(32)

#define	TICK_UNIT_100US

static LIST lFreeTimerNode;
static LIST lUsedTimerNode;

static MSGTIMER timers[MAX_TIMER];
static NODE	timer_nodes[MAX_TIMER];


// 처리 속도를 높이기 위하여 _pTimer 전역 변수 사용할 수 있음
// ------------------------------------
void InitTimerMessage(void)
{
	int32_t node;

	ListInit(&lUsedTimerNode, ASCENT | LINEAR);
	ListInit(&lFreeTimerNode, ASCENT | LINEAR);

	for(node = 0; node < MAX_TIMER; node++)
	{
		// node에 object를 연결해준다.
		timer_nodes[node].pObject = &timers[node];

		// free list에 연결한다
		ListAddNodeTail(&lFreeTimerNode, &timer_nodes[node]);
	}

}

int32_t GetTimerMessage(PMESSAGE pMsg)
{
	PMSGTIMER pTimer;

	// list의 제일 처음 노드만 검사한다.
	if( lUsedTimerNode.m_nCount )
	{	// 처음 노드의 timer obj를 가져온다
		pTimer = (PMSGTIMER)(lUsedTimerNode.m_pNodeHead->pObject);

		// timer가 expire되었는가 검사한다.
		if( (pTimer->expiretimetick - GET_TIME_TICK) > pTimer->interval )
		{	// yes! it expires
			PNODE pNode;

			pMsg->callfunc = pTimer->message.callfunc;
			pMsg->message = pTimer->message.message;
			pMsg->params = pTimer->message.params;

			// 사용한 node를 usednode에서 때어내어
			pNode = ListRemoveHeadNode(&lUsedTimerNode);

			// 다시 사용하기 위하여 freenode에 연결한다
			ListAddNodeTail(&lFreeTimerNode, pNode);

			// 처리 속도를 높이기 위해 다음 timer node의 값을 갖는다
			// if ( lUsedTimerNode.m_pNodeHead )
			// 	_pTimer = (PMSGTIMER)(lUsedTimerNode.m_pNodeHead->pObject);
			// else
			// _pTimer = NULL;

			// Timer event가 발생하였음을 알린다
			return (int32_t)1;
		}
	}

	// 발생한 Timer Event 없음을 알린다
	return (int32_t)0;
}


/* 반드시 interval은 0보다 커야한다 */
PNODE SendTimerMessage(callbackmsg_t hwnd, int32_t msg, int32_t params, uint32_t interval)
{
	PNODE pNewNode;
	PNODE pOldNode;

	PMSGTIMER pTimer;
	PMSGTIMER pOldTimer;

	uint32_t remained_tick;
	uint32_t jiffies_stamp;

#ifdef	DEBUG
if (hwnd == (void*)0)
{
	printf("timer: invalid function null");

	while(1);
}

if ( interval == (uint32_t)0 )
{
	printf("timer: invalid interval");

	while(1);
}
#endif

	if( lFreeTimerNode.m_nCount > 0 )
	{
		// FreeNode의 head를 때어온다
		pNewNode = ListRemoveHeadNode(&lFreeTimerNode);

		pTimer = (PMSGTIMER)(pNewNode->pObject);

#ifdef	TICK_UNIT_100US
		pTimer->interval = (uint32_t)(interval * 10);	// 100usec단위이므로 10으로 곱해 1msec로 바꾼다

#else
		pTimer->interval = (uint32_t)(interval);		// 1msec단위이므로 그대로 사용한다
#endif

		jiffies_stamp = GET_TIME_TICK;					// jiffies를 copy한다
		pTimer->expiretimetick = pTimer->interval + jiffies_stamp;

		// message를 copy한다
		pTimer->message.callfunc = hwnd;
		pTimer->message.message = msg;
		pTimer->message.params = params;

		// lUsedTimerNode list에 pNewNode를 insert한다
		pOldNode = lUsedTimerNode.m_pNodeHead;		// 시작 노드의 포인터를 가져온다

		while( pOldNode )
		{
			pOldTimer = (PMSGTIMER)pOldNode->pObject;
			remained_tick = pOldTimer->expiretimetick - jiffies_stamp;

			if( remained_tick <= pOldTimer->interval )
			{	// 가져온 timer가 expire 된 것이 아닐 때. 반드시 비교할 것
				if ( pTimer->interval < remained_tick )	// pOldNode앞에 pNewNode가 와야 하는가?
				{
					ListInsertNodeBefore(&lUsedTimerNode, pOldNode, pNewNode);

					// 처리 속도를 높이기 위해 다음 timer node의 값을 갖는다
					// if ( lUsedTimerNode.m_pNodeHead )
					// 	_pTimer = (PMSGTIMER)(lUsedTimerNode.m_pNodeHead->pObject);
					// else
					// _pTimer = NULL;

					return pNewNode;
				}
			}

			pOldNode = pOldNode->pNext;
		}

		// 여기 까지 왔으면 끝부분까지 왔으므로 끝에 추가
		ListAddNodeTail(&lUsedTimerNode, pNewNode); // insert tail

		// 처리 속도를 높이기 위해 다음 timer node의 값을 갖는다
		// if ( lUsedTimerNode.m_pNodeHead )
		// 	_pTimer = (PMSGTIMER)(lUsedTimerNode.m_pNodeHead->pObject);
		// else
		// _pTimer = NULL;

		return pNewNode;
	}

#ifdef	DEBUG
	//printf("timer: Not enough timer message node. %d\r\n", lFreeTimerNode.m_nCount);
#endif

	return (void*)0;
}


// pNode를 lUsedTimerNode에서 때어서 lFreeTimerNode에 연결한다
// void RemoveTimerMessage(void *owner, PNODE pRemoveNode)
void RemoveTimerMessage(PNODE *pRemoveNode)
{
//	PMSGTIMER pTimer;

//	pTimer = (PMSGTIMER)(pRemoveNode->pObject);


//	if ( pTimer->message.callfunc == owner )
//	{

#ifdef	DEBUG
		// used node에서 pNode가 있는지 검사한다. (중복 삭제 방지)
		PNODE pNode = lUsedTimerNode.m_pNodeHead;

		while( pNode )
		{
			if ( pNode == *pRemoveNode )
			{	// used list에 remove할 node가 존재할 때
#endif
		// used list에서 노드를 삭제한다
		ListRemoveNodeAt(&lUsedTimerNode, *pRemoveNode);

		// 삭제한 노드를 free list에 덧붙인다
		ListAddNodeTail(&lFreeTimerNode, *pRemoveNode);

		*pRemoveNode = (void*)0;	// NULL;

#ifdef	DEBUG
				break;	// break while
			}
			else
			{
				pNode = pNode->pNext;
			}
		}

		// NULL 이 아니면 잘못 불른것임. 버그 발생의 원인이 됨
		if (*pRemoveNode != NULL )
			while(1);
#endif
//	}

}
