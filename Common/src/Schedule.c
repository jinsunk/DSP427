#include <stdio.h>		// printf
#include <stdlib.h>
#include <string.h>		// memcpy
#include "Schedule.h"
#include "console.h"
#include "board.h"		// GET_TIME_TICK�� ����Ѵ�. jiffies�� ���� �Ǿ� ���� ����

#define	TICK_UNIT_100US

// #define	DEBUG_MSG

int32_t InitSchedule(PLIST pList)
{
	ListInit(pList, ASCENT | LINEAR);

	// ListInit�Ŀ��� defualt������ m_pNodeHead���̴�
	// m_pNodePos�� NULL�� �����ϰ� �Ѵ�
	pList->m_pNodePos = NULL;

	return 0;
}

// �޸𸮸� �����ϰ� ����Ʈ�� �߰��ϴ� �۾��� �Ѵ�
// �� ���۽� 1���� ����Ǿ�� �Ѵ�.
// �ð��� ���� ���� ���� ���� ������� �տ������� ä���
PNODE AddNewSchedule(PLIST pS, PSTAG pT)
{
	PNODE pNewNode, pOldNode;
	PSTAG pNewTag, pOldTag;
	uint32_t jiffies_stamp;

	// Node�� �����Ѵ�
	pNewNode = (PNODE)malloc(sizeof(NODE));
	if ( pNewNode )
	{

#ifdef	DEBUG_MSG
		console_printf("schd: Node=%p\r\n", pNewNode);
#endif

				// ������ Node�� Schedule Tag������ �����Ͽ� �����Ѵ�
		pNewTag = (PSTAG)malloc(sizeof(STAG));
		if ( pNewTag )
		{
			uint32_t remained_tick;

#ifdef	DEBUG_MSG
		console_printf("schd: Tag=%p\r\n", pNewTag);
#endif
			// original delay time
#ifdef	TICK_UNIT_100US
			pNewTag->intervaltime = pT->intervaltime * 10;		//100US �����̹Ƿ� 10�� ���Ѵ�
#else
			pNewTag->intervaltime = pT->intervaltime;
#endif

			// �ʱ� adaptive delay time = intervaltime
			pNewTag->adaptivetime = pNewTag->intervaltime;

			// ���� tick�� �����´�
			jiffies_stamp = GET_TIME_TICK_100US;

			// expired time
			pNewTag->expiretime = (pNewTag->adaptivetime + jiffies_stamp);

			// ������ ������ copy�Ѵ�
			memcpy(pNewTag->frame, pT->frame, MAX_SFRAME*sizeof(uint32_t));

			pNewNode->pObject = pNewTag;

			// list ��� ���� ������ �����Ѵ�
			pOldNode = pS->m_pNodeHead;		// ���� ����� �����͸� �����´�
			while( pOldNode )
			{
				pOldTag = (PSTAG)pOldNode->pObject;
				remained_tick = pOldTag->expiretime - jiffies_stamp;

				if( remained_tick <= (pOldTag->adaptivetime) )	// ���� expire���� �ʾҴٸ�
				{
					if ( pNewTag->adaptivetime < remained_tick )	// ���ο� ������ð��� old�� ���� �ִ� �ð����� �۴ٸ�
					{
						ListInsertNodeBefore(pS, pOldNode, pNewNode);	// old �տ� new �������� ������

						return pNewNode;
					}
				}

				pOldNode = pOldNode->pNext;
			}

			// ���� ���� ������ ���κб��� �����Ƿ� ���� �߰�
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

// �� ���� �������� �����ð��� �����Ѵ�
int32_t ProbeSchedule(PLIST pS)
{
	// ���� ���� �������� �����´�
	if ( pS->m_nCount )
	{
		uint32_t remainedtick;
		PSTAG pTag;

		// Head Node�� ���� �ð��� ���
		pTag= (PSTAG)pS->m_pNodeHead->pObject;

		remainedtick = pTag->expiretime - GET_TIME_TICK_100US;

		if ( remainedtick > pTag->adaptivetime )
			return 0;				// expired�Ǽ� ���� �ִ� �ð��� ����
		else
			// ���� �ִ� �ð��� 1msec ������ return�Ѵ�
#ifdef	TICK_UNIT_100US
			return (remainedtick / 10);
#else
			return (remainedtick);
#endif
	}

	return -1;	// �������� ����
}

int32_t ReadSchedule(PLIST pS, PSTAG pT)
{
	PSTAG pTag;

	// ��� ���� �������� ������
	if ( pS->m_nCount )
	{
		pTag= (PSTAG)pS->m_pNodeHead->pObject;

 		memcpy(pT, pTag, sizeof(STAG));

		return 1;
	}

	return 0;
}

// intervl �ð��� adaptive delay time�� �ִ´�
int32_t UpdatetSchedule(PLIST pS)
{
	if ( pS->m_nCount )
	{
		PNODE pNewNode, pOldNode;
		PSTAG pNewTag, pOldTag;
		uint32_t remained_tick;
		uint32_t jiffies_stamp;

		// �տ��ִ� Node�� ����� �����´�
		pNewNode = ListRemoveHeadNode(pS);
		pNewTag  = pNewNode->pObject;

		// �ð��� default���� �����´�
		pNewTag->adaptivetime = pNewTag->intervaltime;

		jiffies_stamp = GET_TIME_TICK_100US;

		pNewTag->expiretime = (jiffies_stamp + pNewTag->adaptivetime);

		// list ��� ���� ������ �����Ѵ�
		pOldNode = pS->m_pNodeHead;		// ���� ����� �����͸� �����´�
		while( pOldNode )
		{
			pOldTag = (PSTAG)pOldNode->pObject;
			remained_tick = pOldTag->expiretime - jiffies_stamp;

			if( remained_tick <= pOldTag->adaptivetime )	// ���� expire���� �ʾҴٸ�
			{
				if ( pNewTag->adaptivetime < remained_tick )	// ���ο� ������ð��� old�� ���� �ִ� �ð����� �۴ٸ�
				{
					ListInsertNodeBefore(pS, pOldNode, pNewNode);	// old �տ� new �������� ������

					return 0;
				}
			}

			pOldNode = pOldNode->pNext;
		}

		// ���� ���� ������ ���κб��� �����Ƿ� ���� �߰�
		ListAddNodeTail(pS, pNewNode); // insert tail

		return 0;
	}

	return 1;
}


// intervl �ð��� adaptive delay time�� �ִ´�
int32_t UpdatetScheduleDelay(PLIST pS, int32_t interval)
{
	if ( pS->m_nCount )
	{
		PNODE pNewNode, pOldNode;
		PSTAG pNewTag, pOldTag;
		uint32_t jiffies_stamp;
		uint32_t remained_tick;

		// �տ��ִ� Node�� ����� �����´�
		pNewNode = ListRemoveHeadNode(pS);
		pNewTag  = pNewNode->pObject;

		// pNewTag->intervaltime : ���������� ������ �ʰ� �ǵ����� �ʴ´�
		jiffies_stamp = GET_TIME_TICK_100US;

		pNewTag->adaptivetime = interval * 10;		//100US �����̹Ƿ� 10�� ���Ѵ�

		pNewTag->expiretime = (jiffies_stamp + pNewTag->adaptivetime);

		// list ��� ���� ������ �����Ѵ�
		pOldNode = pS->m_pNodeHead;		// ���� ����� �����͸� �����´�
		while( pOldNode )
		{
			pOldTag = (PSTAG)pOldNode->pObject;
			remained_tick = pOldTag->expiretime - jiffies_stamp;

			if( remained_tick <= pOldTag->adaptivetime )	// ���� expire���� �ʾҴٸ�
			{
				if ( pNewTag->adaptivetime < remained_tick )	// ���ο� ������ð��� old�� ���� �ִ� �ð����� �۴ٸ�
				{
					ListInsertNodeBefore(pS, pOldNode, pNewNode);	// old �տ� new �������� ������

					return 0;
				}
			}

			pOldNode = pOldNode->pNext;
		}

		// ���� ���� ������ ���κб��� �����Ƿ� ���� �߰�
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
		// Head Node�� �����´�
		pNode = ListRemoveHeadNode(pS);

		pTag= (PSTAG)pNode->pObject;

		// Tag�� �����
		free(pTag);

		// Node�� �����
		free(pNode);
	}

	// �� �ѹ��� ������� �ʾ��� ��� ������ �߻���Ų��
	// list������ �޸𸮿� �Ҵ�Ǿ��� ������ �����
/*
	if ( pS->lWaitQueue.m_pNodePos )
	{
		// Node�� �����´�
		pNode = pS->lWaitQueue.m_pNodePos;

		pTag= (PSTAG)pNode->pObject;

		// Tag�� �����
		free(pTag);

		// Node�� �����
		free(pNode);
	}
*/
	return 0;
}
