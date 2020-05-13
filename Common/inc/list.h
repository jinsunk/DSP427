#ifndef	_LIST_H_
#define	_LIST_H_

#ifndef _STDINT
#include <stdint.h>
#endif

// list 구성시 property의 내용
#define	LINEAR	0		// 1자형 list
#define	CIRCULA	1		// 환영 list

#define	ASCENT	4		// 오름차순 정렬
#define	DESCENT	5		// 내림차순 정렬

typedef struct node
{
	void *pObject;

	struct node *pPrev;
	struct node *pNext;
} NODE, *PNODE;

typedef struct
{
	PNODE m_pNodeHead;	// list의 시작 node
	PNODE m_pNodeTail;	// list의 끝 node
	PNODE m_pNodePos;	// list의 현재 node

	int32_t	m_nCount;	// list위 node 개수
	int32_t	m_Property;	// list의 특성(환형인가?)
} LIST, *PLIST;

#ifdef __cplusplus
extern "C" {
#endif

extern void ListInit(PLIST pList, int32_t property);

/* NODE 관련 */
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
