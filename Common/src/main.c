#include "sysmsg.h"
#include "timer.h"
#include "procmsg.h"
#include "routine.h"
#include "syscntl.h"

void main(void)
{
	MESSAGE msg;

	// Message�� �ʱ�ȭ �Ѵ�
	InitSystemMessage();
	InitTimerMessage();
	InitMessage();
	InitRoutine();

	// main control process�� �����Ѵ�
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

		// RunRoutine ȣ��� ������ �ݺ����� �۾��� �����Ѵ�
		// ex. DI �Է� ����, key�Է� ����, LCD ȭ�� ����
		RunRoutine();
	}
}
