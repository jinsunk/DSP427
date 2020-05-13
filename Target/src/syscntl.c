#include <stdio.h>

#include "sysmsg.h"
#include "timer.h"
#include "procmsg.h"
#include "routine.h"

#include "board.h"

#include "state.h"
// #include "console.h"

// #define	DEBUG_MSG


#define	LTM_RUNLED		(LM_TIMER+1)

uint32_t syscntl_status;
uint32_t syscntl_diagnosis;

void syscntl_DispatchMessage(int32_t msg, int32_t parmas);

void syscntl_ResetHW(void)
{
	// �⺻ ���� �ϵ��� ����ϱ� ���Ͽ� �ʱ�ȭ �Ѵ�
	board_HardwareInit();

	// �ϵ����� ����ϴ� ��� timer �ʱ�ȭ �Ѵ�
	board_TimerInit();

	// Timer�� �ʱ�ȭ �� �� ����ϴ� ���ͷ�Ʈ�� �ʱ�ȭ�Ѵ�
	board_InterruptInit();

	printf("Hardware Ver. %ld.0\r\n", board_HwVer()+1);
	printf("Sofrware Ver. 1.0\r\n");
	printf("Compiled at %s\r\n", __DATE__);

	// MemoryInit();

	__enable_interrupt();

#ifdef	DEBUG_MSG
	console_printf("syscntl: Reset Hardware\r\n");
#endif

	syscntl_status = ST_HWRESET;

	// Lcd�� �ʱ�ȭ �� ����� �ð��� ��ٷ��ش�
	SendTimerMessage(syscntl_DispatchMessage, LM_TIMER, TM_HWRESET, 500 /* msec */);
}

void syscntl_SetHW(void)
{
#ifdef	DEBUG_MSG
	console_printf("syscntl: Set Hardware\r\n");
#endif

	syscntl_status = ST_HWSET;

	SendTimerMessage(syscntl_DispatchMessage, LM_TIMER, TM_HWSET, 10 /* msec */);
}


void syscntl_ResetSW(void)
{
#ifdef	DEBUG_MSG
	console_printf("syscntl: Reset Software\r\n");
#endif

	syscntl_status = ST_SWRESET;

	SendTimerMessage(syscntl_DispatchMessage, LM_TIMER, TM_SWRESET, 10 /* msec */);
}

void syscntl_SetSW(void)
{
#ifdef	DEBUG_MSG
	console_printf("syscntl: Set Software\r\n");
#endif

	syscntl_status = ST_SWSET;

	SendTimerMessage(syscntl_DispatchMessage, LM_TIMER, TM_SWSET, 10 /* msec */);
}

void syscntl_Config(void)
{
#ifdef	DEBUG_MSG
	console_printf("syscntl: configuration\r\n");
#endif

	// Trigger Run Led
	SendTimerMessage(syscntl_DispatchMessage, LTM_RUNLED, 0, 500 /* msec */);
}

void syscntl_ProcessInit(void)
{

	syscntl_ResetHW();
}

// control process�� ������ �ϵ��� �����Ƿ� ���ͷ�Ʈ�� ���� ó���� ���� �ʴ´�
// void syscntl_DispatchSystemMessage(int32_t sysmsg, int32_t parmas) = null

void syscntl_DispatchMessage(int32_t msg, int32_t params)
{
	// lcd�� �ʱ�ȭ �ɶ����� �ٸ� ���μ����� ���۽õd �ʴ´�

	switch( msg )
	{
		case LM_TIMER :
			switch( params )
			{
				case TM_HWRESET :	// Hardware Reset Timeout �߻�
					syscntl_SetHW();
					break;

				case TM_HWSET :		// Hardware Set Timeout �߻�
					syscntl_ResetSW();
					break;

				case TM_SWRESET :
					syscntl_SetSW();
					break;

				case TM_SWSET :
					syscntl_Config();
					break;

			}
			break;

		case LTM_RUNLED :
			board_TestLed();
			SendTimerMessage(syscntl_DispatchMessage, LTM_RUNLED, 0, 500 /* msec */);
			break;

		default :
			;
	}
}
