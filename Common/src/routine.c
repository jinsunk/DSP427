#include "list.h"
#include "routine.h"

#ifdef DEBUG
#include <stdio.h>
#endif

#define MAX_ROUTINE		8	// �ִ� ���డ���� ����(�������� �ý��� �����ս��� ���� ����)

static LIST lPeriodicRt;	// ���������� ȣ��Ǵ� ��ƾ
static LIST lCyclicRt;		// �ֱ������� �׻� ȣ��Ǵ� ��ƾ

static LIST lFreeRtNode;
static NODE	rt_nodes[MAX_ROUTINE];

void InitRoutine(void)
{
	int32_t node;

	// node�ʱ�ȭ
	ListInit(&lFreeRtNode, ASCENT | LINEAR);

	// routine �ʱ�ȭ
	ListInit(&lCyclicRt, ASCENT | LINEAR);		// ������ ť�� �����
	ListInit(&lPeriodicRt, ASCENT | CIRCULA);	// ������ ť�� �����

	for(node = 0; node < MAX_ROUTINE; node++)
	{
		// node�� object�� �ʱ�ȭ���ش�
		rt_nodes[node].pObject = (void*)0;

		// free list�� �����Ѵ�
		ListAddNodeTail(&lFreeRtNode, &rt_nodes[node]);
	}
}

void RunRoutine(void)
{
	routinef_t pRoutinef;

	// �ֱ������� ȣ���ϴ� ��ƾ�� �����Ѵ�
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
			// �Լ��� ȣ���Ͽ� �����Ѵ�
			pRoutinef();

			lCyclicRt.m_pNodePos = lCyclicRt.m_pNodePos->pNext;
		}
	}

	// ���������� �����ϴ� ��ƾ�� ȣ���Ѵ�
	// ȣ��� ���� �ϳ��� �����ϰ� �������´�
	if ( lPeriodicRt.m_nCount )
	{
		pRoutinef = (routinef_t)(lPeriodicRt.m_pNodePos->pObject);

#ifdef DEBUG
		if (pRoutinef == (void*)0)
			while(1);
#endif
		// �Լ��� ȣ���Ͽ� �����Ѵ�
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
				// CIRCULA���¶� Ư���ϰ� �ٷ��
				ListAddNodeTail(&lPeriodicRt, pNode);

				lPeriodicRt.m_pNodeHead->pPrev = lPeriodicRt.m_pNodeTail;
				lPeriodicRt.m_pNodeTail->pNext = lPeriodicRt.m_pNodeHead;
		}
	}
#ifdef DEBUG
	else
	{
		// MAX_ROUTINE ���� �ø���
		printf("not enough free node\r\n");
		while(1);
	}
#endif

	return 0;
}
