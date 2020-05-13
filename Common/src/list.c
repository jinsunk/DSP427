#include "list.h"

// ����Ʈ ������ �����ϸ� �ݵ�� �ʱ�ȭ�Ͽ��� �Ѵ�.
// property�� ���� �����̳� ������������ ���³��� ����Ϸ��� ����� ��
void ListInit(PLIST pList, int32_t property)
{
	pList->m_pNodeHead = pList->m_pNodeTail = (void*)0;

	pList->m_pNodePos = pList->m_pNodeHead;

	pList->m_nCount = 0;
	pList->m_Property = property;
}

PNODE ListAddNodeHead(PLIST pList, PNODE pNode)
{
	pNode->pPrev = (void*)0;
	pNode->pNext = pList->m_pNodeHead;

	if (pList->m_pNodeHead != (void*)0)
		pList->m_pNodeHead->pPrev = pNode;
	else
		pList->m_pNodeTail = pNode;

	pList->m_pNodeHead = pNode;

	pList->m_nCount++;
	
	return pNode;
}

// �����Ǿ� �ִ� ��带 pList�� ���� ���δ�.
PNODE ListAddNodeTail(PLIST pList, PNODE pNode)
{
	pNode->pPrev = pList->m_pNodeTail;
	pNode->pNext = (void*)0;

	if (pList->m_pNodeTail != (void*)0)
		pList->m_pNodeTail->pNext = pNode;
	else
		pList->m_pNodeHead = pNode;

	pList->m_pNodeTail = pNode;

	pList->m_nCount++;

//	2015-02-12 ���� 3:22:12 �߰�
	if ( pList->m_pNodePos == (void*)0 )
		pList->m_pNodePos = pList->m_pNodeHead;

	return pNode;
}

// pList�� head node�� �����
PNODE ListRemoveHeadNode(PLIST pList)
{
	PNODE pNode = pList->m_pNodeHead;

	pList->m_pNodeHead = pNode->pNext;
	if (pList->m_pNodeHead != (void*)0)
		pList->m_pNodeHead->pPrev = (void*)0;
	else
		pList->m_pNodeTail = (void*)0;

	pList->m_nCount--;

	return pNode;
}

// pList�� Tail node�� �����
PNODE ListRemoveTailNode(PLIST pList)
{
	PNODE pNode = pList->m_pNodeTail;

	pList->m_pNodeTail = pNode->pPrev;
	if (pList->m_pNodeTail != 0)
		pList->m_pNodeTail->pNext = 0;
	else
		pList->m_pNodeHead = 0;

	pList->m_nCount--;

	return pNode;
}

void ListRemoveNodeAt(PLIST pList, PNODE pNode)
{
	// ��������ϴ� ��尡 head�ΰ�?
	if (pNode == pList->m_pNodeHead)
	{
		pList->m_pNodeHead = pNode->pNext;
	}
	else
	{
		pNode->pPrev->pNext = pNode->pNext;
	}

	// ��������ϴ� ��尡 Tail�ΰ�?
	if (pNode == pList->m_pNodeTail)
	{
		pList->m_pNodeTail = pNode->pPrev;
	}
	else
	{
		pNode->pNext->pPrev = pNode->pPrev;
	}

	pList->m_nCount--;
}

/* pOldNode �տ� pNewNode�� �ִ´� */
/* �� �Լ��� Timer�� ������ ������ ���ȴ� */
PNODE ListInsertNodeBefore(PLIST pList, PNODE pOldNode, PNODE pNewNode)
{
	if (pOldNode == (void*)0)
		return ListAddNodeHead(pList, pNewNode); // insert before nothing -> head of the list

	pNewNode->pPrev = pOldNode->pPrev;
	pNewNode->pNext = pOldNode;

	if (pOldNode->pPrev !=  (void*)0)
	{
		pOldNode->pPrev->pNext = pNewNode;
	}
	else
	{
		pList->m_pNodeHead = pNewNode;
	}

	pOldNode->pPrev = pNewNode;

	pList->m_nCount++;

	return pNewNode;
}

/* pOldNode�ڿ� pNewNode�� �ִ´� */
PNODE ListInsertNodeAfter(PLIST pList, PNODE pOldNode, PNODE pNewNode)
{
	if (pOldNode ==  (void*)0)
		return ListAddNodeTail(pList, pNewNode); // insert after nothing -> tail of the list

	pNewNode->pPrev = pOldNode;
	pNewNode->pNext = pOldNode->pNext;

	if (pOldNode->pNext !=  (void*)0)
	{
		pOldNode->pNext->pPrev = pNewNode;
	}
	else
	{
		pList->m_pNodeTail = pNewNode;
	}

	pOldNode->pNext = pNewNode;

	pList->m_nCount++;

	return pNewNode;
}

