#include "list.h"

// 리스트 변수를 생성하면 반드시 초기화하여야 한다.
// property는 오름 차순이나 내림차순등을 나태낼때 사용하려고 만들어 둠
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

// 생성되어 있는 노드를 pList의 끝에 붙인다.
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

//	2015-02-12 오후 3:22:12 추가
	if ( pList->m_pNodePos == (void*)0 )
		pList->m_pNodePos = pList->m_pNodeHead;

	return pNode;
}

// pList의 head node를 때어낸다
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

// pList의 Tail node를 때어낸다
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
	// 지우려고하는 노드가 head인가?
	if (pNode == pList->m_pNodeHead)
	{
		pList->m_pNodeHead = pNode->pNext;
	}
	else
	{
		pNode->pPrev->pNext = pNode->pNext;
	}

	// 지우려고하는 노드가 Tail인가?
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

/* pOldNode 앞에 pNewNode를 넣는다 */
/* 이 함수는 Timer의 스케쥴 관리에 사용된다 */
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

/* pOldNode뒤에 pNewNode를 넣는다 */
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

