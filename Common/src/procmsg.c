#include <string.h>	// memcpy

#include "list.h"
#include "procmsg.h"

#define	MAX_MESSAGES 16

// queue�� �޼����� �����Ѵ�.
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
		// node�� object�� �������ش�.
		msg_nodes[node].pObject = &messages[node];

		// free list�� �����Ѵ�
		ListAddNodeTail(&lFreeMsgNode, &msg_nodes[node]);
	}

}

int32_t SendMessage(callbackmsg_t hprc, int32_t msg, int32_t param, int32_t priority)
{
//	���� �ִ°��� 
//	hprc : handle of process
//	msg  : message
//	param : �׿� option���� message
//	time : �̸޼����� �߻��� �ð�

	PNODE pNode;
	PMESSAGE pMessage;

	// lFreeMsgNode�� ���尡 ������
	if (lFreeMsgNode.m_nCount)
	{
		// lFreeMsgNode�� head�� �����
		pNode = ListRemoveHeadNode(&lFreeMsgNode);

		// �޼����� 1�� word�� ó���ϰ� �Ѵ�.
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

	// priority�� ���� insert�Ѵ�.
	switch( priority )
	{
		case 0 :	// ���� �켱 ������ ���� ��
			ListAddNodeHead(&lUsedMsgNode, pNode);
			break;

		default :
			ListAddNodeTail(&lUsedMsgNode, pNode);
	}

#ifdef	DEBUG		// message�� �ִ� �󸶱��� �����Ͽ��°� ����Ѵ�.
		if( max_message_count < lUsedMsgNode.m_nCount)
			max_message_count = lUsedMsgNode.m_nCount;
#endif

	return (int32_t)0;
}


// �׻� list�� �տ��� �����´�
int32_t GetMessage(PMESSAGE pMsg)
{
	PNODE pNode;

	if (lUsedMsgNode.m_nCount)
	{
		// �޼����� copy�Ѵ�
		memcpy(pMsg, lUsedMsgNode.m_pNodeHead->pObject, MESSAGE_SIZE8);

		// ��忡�� UsedList���� �����Ѵ�.
		pNode = ListRemoveHeadNode(&lUsedMsgNode);

		// ������ ��带 FreeList�� ���Ѵ�.
		ListAddNodeTail(&lFreeMsgNode, pNode);

		return (int32_t)1;
	}

	return (int32_t)0;	// �޼��� ����
}





