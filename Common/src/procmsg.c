#include <string.h>	// memcpy

#include "list.h"
#include "procmsg.h"

#define	MAX_MESSAGES 16

// queue에 메세지를 관리한다.
static MESSAGE messages[MAX_MESSAGES];
static NODE	   msg_nodes[MAX_MESSAGES];
static LIST lUsedMsgNode;
static LIST lFreeMsgNode;

#ifdef	DEBUG
int32_t	max_message_count;
#endif

void InitMessage(void)
{
	int32_t node;

#ifdef	DEBUG
	max_message_count = (int32_t)0;
#endif

	ListInit(&lUsedMsgNode, ASCENT | LINEAR);
	ListInit(&lFreeMsgNode, ASCENT | LINEAR);

	for(node = 0; node < MAX_MESSAGES; node++)
	{
		// node에 object를 연결해준다.
		msg_nodes[node].pObject = &messages[node];

		// free list에 연결한다
		ListAddNodeTail(&lFreeMsgNode, &msg_nodes[node]);
	}

}

int32_t SendMessage(callbackmsg_t hprc, int32_t msg, int32_t param, int32_t priority)
{
//	집어 넣는것은 
//	hprc : handle of process
//	msg  : message
//	param : 그외 option적인 message
//	time : 이메세지가 발생한 시간

	PNODE pNode;
	PMESSAGE pMessage;

	// lFreeMsgNode에 빈노드가 있으면
	if (lFreeMsgNode.m_nCount)
	{
		// lFreeMsgNode의 head를 때어낸다
		pNode = ListRemoveHeadNode(&lFreeMsgNode);

		// 메세지를 1개 word로 처리하게 한다.
		pMessage = (PMESSAGE)(pNode->pObject);

		pMessage->callfunc = hprc;
		pMessage->message = msg;
		pMessage->params = param;

#ifdef	__MSG_STAMP_TIME__
		pMessage->seconds = hprc;
		pMessage->subsecs = hprc;
#endif
	}
	else return (int32_t)1;		// return with error: no free node

	// priority를 보고 insert한다.
	switch( priority )
	{
		case 0 :	// 가장 우선 순위가 높을 때
			ListAddNodeHead(&lUsedMsgNode, pNode);
			break;

		default :
			ListAddNodeTail(&lUsedMsgNode, pNode);
	}

#ifdef	DEBUG		// message가 최대 얼마까지 증가하였는가 기록한다.
		if( max_message_count < lUsedMsgNode.m_nCount)
			max_message_count = lUsedMsgNode.m_nCount;
#endif

	return (int32_t)0;
}


// 항상 list의 앞에서 가져온다
int32_t GetMessage(PMESSAGE pMsg)
{
	PNODE pNode;

	if (lUsedMsgNode.m_nCount)
	{
		// 메세지를 copy한다
		memcpy(pMsg, lUsedMsgNode.m_pNodeHead->pObject, MESSAGE_SIZE8);

		// 노드에서 UsedList에서 제거한다.
		pNode = ListRemoveHeadNode(&lUsedMsgNode);

		// 제거한 노드를 FreeList에 더한다.
		ListAddNodeTail(&lFreeMsgNode, pNode);

		return (int32_t)1;
	}

	return (int32_t)0;	// 메세지 없음
}





