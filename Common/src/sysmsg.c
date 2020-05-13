#include "sysmsg.h"

#define	MAX_SYSMSGS	16

static MESSAGE _sysmessages[MAX_SYSMSGS];
static PMESSAGE _msg_wr_ptr;
static PMESSAGE _msg_rd_ptr;

#ifdef	DEBUG
	static int32_t	_n_msg;
	static int32_t	_max_sysmsg;
#endif

void InitSystemMessage(void)
{
#ifdef	DEBUG
	_n_msg = 0;
	_max_sysmsg = 0;
#endif

	_msg_wr_ptr = _msg_rd_ptr = _sysmessages;
}

uint8_t SendSystemMessage(callbackmsg_t hprc, int32_t msg, int32_t param)
{
	PMESSAGE new_sysmsg;

#ifdef	DEBUG
	while(hprc == (void*)0);		// bug�� ��� ���Ͽ� �ӽ÷� �����Ͽ���.

	_n_msg++;

	if( _max_sysmsg < _n_msg )
		_max_sysmsg = _n_msg;		// �޼��� �ִ� ������ �����Ѵ�
#endif

	// �� �Լ��� interrupt���� ȣ��Ǵµ�
	// �켱 ������ ���� ��ó��Ʈ�� �߻���
	// �ߺ��ؼ� ȣ��Ǵ� ������ �߻��� ��
	// �����Ƿ� critical section�� ������
	// ��쿡�� ���ͷ�Ʈ�� disable ��Ų��

	// enter critical section
	// spin lock irq
	__disable_interrupt();

	new_sysmsg = _msg_wr_ptr++;

	if( _msg_wr_ptr == (_sysmessages+MAX_SYSMSGS) )
		_msg_wr_ptr = _sysmessages;

	// exit critical section
	// spin unlock irq
	__enable_interrupt();

#ifdef	__MSG_STAMP_TIME__
	new_sysmsg->seconds = hprc;	// �ʴ����� copy
	new_sysmsg->subsecs = hprc;	// sub �ʴ����� copy�Ѵ�
#endif

	new_sysmsg->callfunc = hprc;
	new_sysmsg->message = msg;
	new_sysmsg->params = param;

	return (uint8_t)0;
}

// ��ϵ� �޼����� ������ �޼����� ���� �����͸� ����
// ������ �޼����� ������ NULL�� �����Ѵ�
uint8_t GetSystemMessage(PMESSAGE pMsg)
{
	if ( _msg_rd_ptr != _msg_wr_ptr )
	{
#ifdef	DEBUG
		_n_msg--;
#endif
		pMsg->callfunc = _msg_rd_ptr->callfunc;
		pMsg->message = _msg_rd_ptr->message;
		pMsg->params = _msg_rd_ptr->params;

#ifdef	__MSG_STAMP_TIME__
		pMsg->seconds = _msg_rd_ptr->seconds;
		pMsg->subsecs = _msg_rd_ptr>subsecs;
#endif

		// message pointer�� �����Ѵ�
		_msg_rd_ptr++;

		if( _msg_rd_ptr == (_sysmessages+MAX_SYSMSGS) )
			_msg_rd_ptr = _sysmessages;

		// ������ message ������ �˸���
		return (uint8_t)1;
	}

	// ������ message ������ �˸���
	return (uint8_t)0;
}
