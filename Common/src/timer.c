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


// ó�� �ӵ��� ���̱� ���Ͽ� _pTimer ���� ���� ����� �� ����
// ------------------------------------
void InitTimerMessage(void)
{
	int32_t node;

	ListInit(&lUsedTimerNode, ASCENT | LINEAR);
	ListInit(&lFreeTimerNode, ASCENT | LINEAR);

	for(node = 0; node < MAX_TIMER; node++)
	{
		// node�� object�� �������ش�.
		timer_nodes[node].pObject = &timers[node];

		// free list�� �����Ѵ�
		ListAddNodeTail(&lFreeTimerNode, &timer_nodes[node]);
	}

}

int32_t GetTimerMessage(PMESSAGE pMsg)
{
	PMSGTIMER pTimer;

	// list�� ���� ó�� ��常 �˻��Ѵ�.
	if( lUsedTimerNode.m_nCount )
	{	// ó�� ����� timer obj�� �����´�
		pTimer = (PMSGTIMER)(lUsedTimerNode.m_pNodeHead->pObject);

		// timer�� expire�Ǿ��°� �˻��Ѵ�.
		if( (pTimer->expiretimetick - GET_TIME_TICK) > pTimer->interval )
		{	// yes! it expires
			PNODE pNode;

			pMsg->callfunc = pTimer->message.callfunc;
			pMsg->message = pTimer->message.message;
			pMsg->params = pTimer->message.params;

			// ����� node�� usednode���� �����
			pNode = ListRemoveHeadNode(&lUsedTimerNode);

			// �ٽ� ����ϱ� ���Ͽ� freenode�� �����Ѵ�
			ListAddNodeTail(&lFreeTimerNode, pNode);

			// ó�� �ӵ��� ���̱� ���� ���� timer node�� ���� ���´�
			// if ( lUsedTimerNode.m_pNodeHead )
			// 	_pTimer = (PMSGTIMER)(lUsedTimerNode.m_pNodeHead->pObject);
			// else
			// _pTimer = NULL;

			// Timer event�� �߻��Ͽ����� �˸���
			return (int32_t)1;
		}
	}

	// �߻��� Timer Event ������ �˸���
	return (int32_t)0;
}


/* �ݵ�� interval�� 0���� Ŀ���Ѵ� */
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
		// FreeNode�� head�� ����´�
		pNewNode = ListRemoveHeadNode(&lFreeTimerNode);

		pTimer = (PMSGTIMER)(pNewNode->pObject);

#ifdef	TICK_UNIT_100US
		pTimer->interval = (uint32_t)(interval * 10);	// 100usec�����̹Ƿ� 10���� ���� 1msec�� �ٲ۴�

#else
		pTimer->interval = (uint32_t)(interval);		// 1msec�����̹Ƿ� �״�� ����Ѵ�
#endif

		jiffies_stamp = GET_TIME_TICK;					// jiffies�� copy�Ѵ�
		pTimer->expiretimetick = pTimer->interval + jiffies_stamp;

		// message�� copy�Ѵ�
		pTimer->message.callfunc = hwnd;
		pTimer->message.message = msg;
		pTimer->message.params = params;

		// lUsedTimerNode list�� pNewNode�� insert�Ѵ�
		pOldNode = lUsedTimerNode.m_pNodeHead;		// ���� ����� �����͸� �����´�

		while( pOldNode )
		{
			pOldTimer = (PMSGTIMER)pOldNode->pObject;
			remained_tick = pOldTimer->expiretimetick - jiffies_stamp;

			if( remained_tick <= pOldTimer->interval )
			{	// ������ timer�� expire �� ���� �ƴ� ��. �ݵ�� ���� ��
				if ( pTimer->interval < remained_tick )	// pOldNode�տ� pNewNode�� �;� �ϴ°�?
				{
					ListInsertNodeBefore(&lUsedTimerNode, pOldNode, pNewNode);

					// ó�� �ӵ��� ���̱� ���� ���� timer node�� ���� ���´�
					// if ( lUsedTimerNode.m_pNodeHead )
					// 	_pTimer = (PMSGTIMER)(lUsedTimerNode.m_pNodeHead->pObject);
					// else
					// _pTimer = NULL;

					return pNewNode;
				}
			}

			pOldNode = pOldNode->pNext;
		}

		// ���� ���� ������ ���κб��� �����Ƿ� ���� �߰�
		ListAddNodeTail(&lUsedTimerNode, pNewNode); // insert tail

		// ó�� �ӵ��� ���̱� ���� ���� timer node�� ���� ���´�
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


// pNode�� lUsedTimerNode���� ��� lFreeTimerNode�� �����Ѵ�
// void RemoveTimerMessage(void *owner, PNODE pRemoveNode)
void RemoveTimerMessage(PNODE *pRemoveNode)
{
//	PMSGTIMER pTimer;

//	pTimer = (PMSGTIMER)(pRemoveNode->pObject);


//	if ( pTimer->message.callfunc == owner )
//	{

#ifdef	DEBUG
		// used node���� pNode�� �ִ��� �˻��Ѵ�. (�ߺ� ���� ����)
		PNODE pNode = lUsedTimerNode.m_pNodeHead;

		while( pNode )
		{
			if ( pNode == *pRemoveNode )
			{	// used list�� remove�� node�� ������ ��
#endif
		// used list���� ��带 �����Ѵ�
		ListRemoveNodeAt(&lUsedTimerNode, *pRemoveNode);

		// ������ ��带 free list�� �����δ�
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

		// NULL �� �ƴϸ� �߸� �Ҹ�����. ���� �߻��� ������ ��
		if (*pRemoveNode != NULL )
			while(1);
#endif
//	}

}
