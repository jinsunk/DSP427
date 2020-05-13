#ifndef	_LIST_H_
#define	_LIST_H_

#ifndef _STDINT
#include <stdint.h>
#endif

// list ������ property�� ����
#define	LINEAR	0		// 1���� list
#define	CIRCULA	1		// ȯ�� list

#define	ASCENT	4		// �������� ����
#define	DESCENT	5		// �������� ����

typedef struct node
{
	void *pObject;

	struct node *pPrev;
	struct node *pNext;
} NODE, *PNODE;

typedef struct
{
	PNODE m_pNodeHead;	// list�� ���� node
	PNODE m_pNodeTail;	// list�� �� node
	PNODE m_pNodePos;	// list�� ���� node

	int32_t	m_nCount;	// list�� node ����
	int32_t	m_Property;	// list�� Ư��(ȯ���ΰ�?)
} LIST, *PLIST;

#ifdef __cplusplus
extern "C" {
#endif

extern void ListInit(PLIST pList, int32_t property);

/* NODE ���� */
extern PNODE ListAddNodeHead(PLIST pList, PNODE pNode);
extern PNODE ListAddNodeTail(PLIST pList, PNODE pNode);

extern PNODE ListRemoveHeadNode(PLIST pList);
extern PNODE ListRemoveTailNode(PLIST pList);

extern PNODE ListInsertNodeBefore(PLIST pList, PNODE pOldNode, PNODE pNewNode);
extern PNODE ListInsertNodeAfter(PLIST pList, PNODE pOldNode, PNODE pNewNode);

extern void ListRemoveNodeAt(PLIST pList, PNODE pNode);

#ifdef __cplusplus
}
#endif

#endif	/* _LIST_H_ */
