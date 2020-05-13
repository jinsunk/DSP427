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
	// 기본 적인 하드웨어를 사용하기 위하여 초기화 한다
	board_HardwareInit();

	// 하드웨어에서 사용하는 모든 timer 초기화 한다
	board_TimerInit();

	// Timer가 초기화 된 후 사용하는 인터럽트를 초기화한다
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

	// Lcd가 초기화 될 충분한 시간을 기다려준다
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

// control process는 별도의 하드웨어가 없으므로 인터럽트에 의한 처리는 하지 않는다
// void syscntl_DispatchSystemMessage(int32_t sysmsg, int32_t parmas) = null

void syscntl_DispatchMessage(int32_t msg, int32_t params)
{
	// lcd가 초기화 될때까지 다른 프로세서를 동작시킺 않는다

	switch( msg )
	{
		case LM_TIMER :
			switch( params )
			{
				case TM_HWRESET :	// Hardware Reset Timeout 발생
					syscntl_SetHW();
					break;

				case TM_HWSET :		// Hardware Set Timeout 발생
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
