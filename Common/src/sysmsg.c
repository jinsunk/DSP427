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
	while(hprc == (void*)0);		// bug를 잡기 위하여 임시로 삽임하였음.

	_n_msg++;

	if( _max_sysmsg < _n_msg )
		_max_sysmsg = _n_msg;		// 메세지 최대 개수를 저장한다
#endif

	// 이 함수는 interrupt에서 호출되는데
	// 우선 순위가 높은 인처럽트가 발생시
	// 중복해서 호출되는 문제가 발생할 수
	// 있으므로 critical section을 수핼할
	// 경우에는 인터럽트를 disable 시킨다

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
	new_sysmsg->seconds = hprc;	// 초단위를 copy
	new_sysmsg->subsecs = hprc;	// sub 초단위를 copy한다
#endif

	new_sysmsg->callfunc = hprc;
	new_sysmsg->message = msg;
	new_sysmsg->params = param;

	return (uint8_t)0;
}

// 등록된 메세지가 있으면 메세지의 시작 포인터를 리턴
// 수행할 메세지가 없으면 NULL을 리턴한다
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

		// message pointer를 갱신한다
		_msg_rd_ptr++;

		if( _msg_rd_ptr == (_sysmessages+MAX_SYSMSGS) )
			_msg_rd_ptr = _sysmessages;

		// 전달할 message 있음을 알린다
		return (uint8_t)1;
	}

	// 전달할 message 없음을 알린다
	return (uint8_t)0;
}
