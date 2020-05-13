#include "sysmsg.h"
#include "timer.h"
#include "procmsg.h"
#include "routine.h"
#include "syscntl.h"

void main(void)
{
	MESSAGE msg;

	// Message를 초기화 한다
	InitSystemMessage();
	InitTimerMessage();
	InitMessage();
	InitRoutine();

	// main control process를 실행한다
	syscntl_ProcessInit();

	while( 1 )
	{

		// dispatch message from Hardware process
		if (GetSystemMessage(&msg))
		{
			msg.callfunc(msg.message, msg.params);
		};

		// dispatch message from Timer process
		if (GetTimerMessage(&msg))
		{
			msg.callfunc(msg.message, msg.params);
		};

		// dispatch message from Software process
		if (GetMessage(&msg))
		{
			msg.callfunc(msg.message, msg.params);
		};

		// RunRoutine 호출될 때마다 반복적인 작업을 수행한다
		// ex. DI 입력 갱신, key입력 갱신, LCD 화면 갱신
		RunRoutine();
	}
}
