#include "list.h"
#include "routine.h"

#ifdef DEBUG
#include <stdio.h>
#endif

#define MAX_ROUTINE		8	// 최대 수행가능한 숫자(많아지면 시스템 퍼포먼스에 좋지 않음)

static LIST lPeriodicRt;	// 간헐적으로 호출되는 루틴
static LIST lCyclicRt;		// 주기적으로 항상 호출되는 루틴

static LIST lFreeRtNode;
static NODE	rt_nodes[MAX_ROUTINE];

void InitRoutine(void)
{
	int32_t node;

	// node초기화
	ListInit(&lFreeRtNode, ASCENT | LINEAR);

	// routine 초기화
	ListInit(&lCyclicRt, ASCENT | LINEAR);		// 선형의 큐를 만든다
	ListInit(&lPeriodicRt, ASCENT | CIRCULA);	// 원형의 큐를 만든다

	for(node = 0; node < MAX_ROUTINE; node++)
	{
		// node에 object를 초기화해준다
		rt_nodes[node].pObject = (void*)0;

		// free list에 연결한다
		ListAddNodeTail(&lFreeRtNode, &rt_nodes[node]);
	}
}

void RunRoutine(void)
{
	routinef_t pRoutinef;

	// 주기적으로 호출하는 루틴을 실행한다
	if ( lCyclicRt.m_nCount )
	{
		lCyclicRt.m_pNodePos = lCyclicRt.m_pNodeHead;

		while( lCyclicRt.m_pNodePos )
		{
			pRoutinef = (routinef_t)(lCyclicRt.m_pNodePos->pObject);

#ifdef DEBUG
			if (pRoutinef == (void*)0)
				while(1);
#endif
			// 함수를 호출하여 수행한다
			pRoutinef();

			lCyclicRt.m_pNodePos = lCyclicRt.m_pNodePos->pNext;
		}
	}

	// 간헐적으로 수행하는 루틴을 호출한다
	// 호출될 때에 하나만 수행하고 빠져나온다
	if ( lPeriodicRt.m_nCount )
	{
		pRoutinef = (routinef_t)(lPeriodicRt.m_pNodePos->pObject);

#ifdef DEBUG
		if (pRoutinef == (void*)0)
			while(1);
#endif
		// 함수를 호출하여 수행한다
		pRoutinef();

		lPeriodicRt.m_pNodePos = lPeriodicRt.m_pNodePos->pNext;
	}
}

int32_t InsertRoutine(int32_t rt_type, routinef_t rt_func)
{
	PNODE pNode;

	pNode = ListRemoveHeadNode(&lFreeRtNode);

	if ( pNode )
	{
		pNode->pObject = (void*)rt_func;

		switch( rt_type )
		{
			case RT_CYCLIC :
				ListAddNodeTail(&lCyclicRt, pNode);
				break;

			// case RT_PERIODIC :
			//	break;

			default :
				// CIRCULA형태라 특수하게 다룬다
				ListAddNodeTail(&lPeriodicRt, pNode);

				lPeriodicRt.m_pNodeHead->pPrev = lPeriodicRt.m_pNodeTail;
				lPeriodicRt.m_pNodeTail->pNext = lPeriodicRt.m_pNodeHead;
		}
	}
#ifdef DEBUG
	else
	{
		// MAX_ROUTINE 값을 올린다
		printf("not enough free node\r\n");
		while(1);
	}
#endif

	return 0;
}
