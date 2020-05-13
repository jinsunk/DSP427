#include <stdio.h>		// printf
#include <stdlib.h>
#include <string.h>		// memcpy
#include "Schedule.h"
#include "console.h"
#include "board.h"		// GET_TIME_TICK를 사용한다. jiffies가 정의 되어 있지 않음

#define	TICK_UNIT_100US

// #define	DEBUG_MSG

int32_t InitSchedule(PLIST pList)
{
	ListInit(pList, ASCENT | LINEAR);

	// ListInit후에는 defualt값으로 m_pNodeHead값이다
	// m_pNodePos를 NULL로 시작하게 한다
	pList->m_pNodePos = NULL;

	return 0;
}

// 메모리를 생성하고 리스트에 추가하는 작업을 한다
// 즉 시작시 1번만 수행되어야 한다.
// 시간을 보고 가장 작은 값을 순서대로 앞에서부터 채운다
PNODE AddNewSchedule(PLIST pS, PSTAG pT)
{
	PNODE pNewNode, pOldNode;
	PSTAG pNewTag, pOldTag;
	uint32_t jiffies_stamp;

	// Node를 생성한다
	pNewNode = (PNODE)malloc(sizeof(NODE));
	if ( pNewNode )
	{

#ifdef	DEBUG_MSG
		console_printf("schd: Node=%p\r\n", pNewNode);
#endif

				// 생성된 Node에 Schedule Tag변수를 생성하여 연결한다
		pNewTag = (PSTAG)malloc(sizeof(STAG));
		if ( pNewTag )
		{
			uint32_t remained_tick;

#ifdef	DEBUG_MSG
		console_printf("schd: Tag=%p\r\n", pNewTag);
#endif
			// original delay time
#ifdef	TICK_UNIT_100US
			pNewTag->intervaltime = pT->intervaltime * 10;		//100US 단위이므로 10을 곱한다
#else
			pNewTag->intervaltime = pT->intervaltime;
#endif

			// 초기 adaptive delay time = intervaltime
			pNewTag->adaptivetime = pNewTag->intervaltime;

			// 현재 tick을 가져온다
			jiffies_stamp = GET_TIME_TICK_100US;

			// expired time
			pNewTag->expiretime = (pNewTag->adaptivetime + jiffies_stamp);

			// 스케쥴 내용을 copy한다
			memcpy(pNewTag->frame, pT->frame, MAX_SFRAME*sizeof(uint32_t));

			pNewNode->pObject = pNewTag;

			// list 어디에 집어 넣을지 결정한다
			pOldNode = pS->m_pNodeHead;		// 시작 노드의 포인터를 가져온다
			while( pOldNode )
			{
				pOldTag = (PSTAG)pOldNode->pObject;
				remained_tick = pOldTag->expiretime - jiffies_stamp;

				if( remained_tick <= (pOldTag->adaptivetime) )	// 아직 expire되지 않았다면
				{
					if ( pNewTag->adaptivetime < remained_tick )	// 새로운 스케쥴시간이 old의 남아 있는 시간보다 작다면
					{
						ListInsertNodeBefore(pS, pOldNode, pNewNode);	// old 앞에 new 스케쥴을 넣은다

						return pNewNode;
					}
				}

				pOldNode = pOldNode->pNext;
			}

			// 여기 까지 왔으면 끝부분까지 왔으므로 끝에 추가
			ListAddNodeTail(pS, pNewNode); // attach at tail

			return pNewNode;
		}
		else
		{
			printf("Schedule: halt! not enough memory for tag\r\n");

			while(1);
		}
	}
	else
	{
		printf("Schedule: halt! not enough memory for node\r\n");

		while(1);
	}

//	return (void*)0;
}

// 맨 앞의 스케쥴의 남은시간을 리턴한다
int32_t ProbeSchedule(PLIST pS)
{
	// 제일 앞의 스케쥴을 가져온다
	if ( pS->m_nCount )
	{
		uint32_t remainedtick;
		PSTAG pTag;

		// Head Node의 남은 시간을 계산
		pTag= (PSTAG)pS->m_pNodeHead->pObject;

		remainedtick = pTag->expiretime - GET_TIME_TICK_100US;

		if ( remainedtick > pTag->adaptivetime )
			return 0;				// expired되서 남아 있는 시간이 없음
		else
			// 남아 있는 시간을 1msec 단위로 return한다
#ifdef	TICK_UNIT_100US
			return (remainedtick / 10);
#else
			return (remainedtick);
#endif
	}

	return -1;	// 스케쥴이 없음
}

int32_t ReadSchedule(PLIST pS, PSTAG pT)
{
	PSTAG pTag;

	// 대기 중인 스케쥴이 있으면
	if ( pS->m_nCount )
	{
		pTag= (PSTAG)pS->m_pNodeHead->pObject;

 		memcpy(pT, pTag, sizeof(STAG));

		return 1;
	}

	return 0;
}

// intervl 시간을 adaptive delay time에 넣는다
int32_t UpdatetSchedule(PLIST pS)
{
	if ( pS->m_nCount )
	{
		PNODE pNewNode, pOldNode;
		PSTAG pNewTag, pOldTag;
		uint32_t remained_tick;
		uint32_t jiffies_stamp;

		// 앞에있던 Node를 때어내어 가져온다
		pNewNode = ListRemoveHeadNode(pS);
		pNewTag  = pNewNode->pObject;

		// 시간은 default값을 가져온다
		pNewTag->adaptivetime = pNewTag->intervaltime;

		jiffies_stamp = GET_TIME_TICK_100US;

		pNewTag->expiretime = (jiffies_stamp + pNewTag->adaptivetime);

		// list 어디에 집어 넣을지 결정한다
		pOldNode = pS->m_pNodeHead;		// 시작 노드의 포인터를 가져온다
		while( pOldNode )
		{
			pOldTag = (PSTAG)pOldNode->pObject;
			remained_tick = pOldTag->expiretime - jiffies_stamp;

			if( remained_tick <= pOldTag->adaptivetime )	// 아직 expire되지 않았다면
			{
				if ( pNewTag->adaptivetime < remained_tick )	// 새로운 스케쥴시간이 old의 남아 있는 시간보다 작다면
				{
					ListInsertNodeBefore(pS, pOldNode, pNewNode);	// old 앞에 new 스케쥴을 넣은다

					return 0;
				}
			}

			pOldNode = pOldNode->pNext;
		}

		// 여기 까지 왔으면 끝부분까지 왔으므로 끝에 추가
		ListAddNodeTail(pS, pNewNode); // insert tail

		return 0;
	}

	return 1;
}


// intervl 시간을 adaptive delay time에 넣는다
int32_t UpdatetScheduleDelay(PLIST pS, int32_t interval)
{
	if ( pS->m_nCount )
	{
		PNODE pNewNode, pOldNode;
		PSTAG pNewTag, pOldTag;
		uint32_t jiffies_stamp;
		uint32_t remained_tick;

		// 앞에있던 Node를 때어내어 가져온다
		pNewNode = ListRemoveHeadNode(pS);
		pNewTag  = pNewNode->pObject;

		// pNewTag->intervaltime : 오리지날은 변하지 않게 건들이지 않는다
		jiffies_stamp = GET_TIME_TICK_100US;

		pNewTag->adaptivetime = interval * 10;		//100US 단위이므로 10을 곱한다

		pNewTag->expiretime = (jiffies_stamp + pNewTag->adaptivetime);

		// list 어디에 집어 넣을지 결정한다
		pOldNode = pS->m_pNodeHead;		// 시작 노드의 포인터를 가져온다
		while( pOldNode )
		{
			pOldTag = (PSTAG)pOldNode->pObject;
			remained_tick = pOldTag->expiretime - jiffies_stamp;

			if( remained_tick <= pOldTag->adaptivetime )	// 아직 expire되지 않았다면
			{
				if ( pNewTag->adaptivetime < remained_tick )	// 새로운 스케쥴시간이 old의 남아 있는 시간보다 작다면
				{
					ListInsertNodeBefore(pS, pOldNode, pNewNode);	// old 앞에 new 스케쥴을 넣은다

					return 0;
				}
			}

			pOldNode = pOldNode->pNext;
		}

		// 여기 까지 왔으면 끝부분까지 왔으므로 끝에 추가
		ListAddNodeTail(pS, pNewNode); // insert tail

		return 0;
	}

	return 1;
}


int32_t DistroySchedule(PLIST pS)
{
	PNODE pNode;
	PSTAG pTag;

	while( pS->m_nCount )
	{
		// Head Node를 가져온다
		pNode = ListRemoveHeadNode(pS);

		pTag= (PSTAG)pNode->pObject;

		// Tag를 지운다
		free(pTag);

		// Node를 지운다
		free(pNode);
	}

	// 즉 한번도 수행되지 않았을 경우 문제를 발생시킨다
	// list없지만 메모리에 할당되었던 변수를 지운다
/*
	if ( pS->lWaitQueue.m_pNodePos )
	{
		// Node를 가져온다
		pNode = pS->lWaitQueue.m_pNodePos;

		pTag= (PSTAG)pNode->pObject;

		// Tag를 지운다
		free(pTag);

		// Node를 지운다
		free(pNode);
	}
*/
	return 0;
}
