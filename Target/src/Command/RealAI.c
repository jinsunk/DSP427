#include <string.h>		// memset
#include <stdlib.h>		// NULL, malloc

#include "sysmsg.h"
#include "timer.h"
#include "procmsg.h"

#include "getopt.h"
#include "debug.h"		// callback �Լ� ����� ���Ͽ�
#include "command.h"
#include "signal.h"
// #define	DEBUG_MSG
static callbackmsg_t __debug_callbackf;
//extern RealDataStruct AI_Data;
typedef struct {
	//int32_t mode;
	//int32_t status;

	int32_t argc;
	// char *argvf[];
	PNODE	tnode;
	// �θ� �����Ѵ�
	callbackmsg_t parentf;

	callbackf_tranceive recvf;
	callbackf_tranceive sendf;
	// �ڽĿ� ���� ���� ���̽�
}  CmdSetAI_t, *PCmdSetAI_t;

void RealAI_DispatchMessage(int32_t msg, int32_t params);
void RealAI_Entry(int argc, char *argv[]);
COMMAND cmd_RealAI = { "analogData", RealAI_Entry, 0 };

void debug__setcallbackf(callbackmsg_t new_func)
{
	__debug_callbackf = new_func;
}
void RealAI_Exit(void)
{
	//CmdSet_Debug pCmd_t;
	PCmdSetAI_t pCmd = (PCmdSetAI_t)pDynamicCmdVariable;

	// �θ� �����Ѵ�
	debug_setcallbackf(pCmd->parentf);
	
		  debug__setcallbackf(pCmd->parentf);
	// �θ𿡰� �۾��� �������� �˸���
	SendMessage(pCmd->parentf, SM_CHILD_CLOSED, 0, 9);

#ifdef	DEBUG_MSG
	debug_printf("Real Analog Input: free var=0x%p\r\n", pCmd);
#endif

	// heap ������ �Ҵ�� ������ �����
	if ( pCmd ) free(pCmd);
}

void RealAI_Entry(int argc, char *argv[])
{
/* �ݵ�� ���� �� �κ� */
	// �ʿ��� ������ heap ������ �Ҵ��Ѵ�
	PCmdSetAI_t pCmd = (PCmdSetAI_t)malloc(sizeof(CmdSetAI_t));

#ifdef	DEBUG_MSG
	debug_printf("Real Analog Input: malloc var=0x%p\r\n", pCmd);
#endif

	// Dynamic ������ ���� ������ �����Ѵ�
	pDynamicCmdVariable = pCmd;		// defined at command.h

	// �Ҵ�� ������ ��� �ʱ�ȭ �Ѵ�
	if ( pCmd )	memset(pCmd, 0, sizeof(CmdSetAI_t) );

	// �θ� �����Ѵ�
	pCmd->parentf = debug_getcallbackf();

	// �ۼ��� �̺�Ʈ�� �� �Լ��� �����Ѵ�
	debug_setcallbackf(RealAI_DispatchMessage);
	debug__setcallbackf(RealAI_DispatchMessage);
	// memset ��ɾ�� � �ɼǵ��� �ִ°�?
	// ������ �޸𸮿� �����͸� ������ �����Ѵ�


	debug_printf("Analog Input \n\r");

	
	//debug_printf("\n");
	

	// �غ� �Ϸ� �Ǿ����� �˸���
	SendMessage(RealAI_DispatchMessage,SM_INITIALIZED, 0, 9);
}



void RealAI_DispatchMessage(int32_t msg, int32_t params)
{
	PCmdSetAI_t pCmd;
	 pCmd=(PCmdSetAI_t)pDynamicCmdVariable ;	
	switch( msg )
	{
		case SM_RECEIVED :	
			RealAI_Exit();
			break;

		case SM_TRANSMITTED :
			// ���� �۽��� ���Ͽ�
			// ���� �۽��� ������ ������
			// ���� �۽��� ������ 
			pCmd->tnode = SendTimerMessage(RealAI_DispatchMessage, LTM_DELAY, 0, 1000 /* msec */);
			break;

		case SM_INITIALIZED :
		  
		   for(int i=0;i<16;i++){
			if(i%5==0)  	 debug_printf("\r\n");
			 debug_printf("%d:%7.2f\t",i,AI_Data.DFTvalue[i].mag);
		 }
		debug_printf("\r\n");
		//	SendTimerMessage(__debug_callbackf, SM_INITIALIZED, 0, 1000);
			break;

		case SM_CLOSED :
			RealAI_Exit();
			break;

		//		case SM_CHILD_CLOSED :
		// �ڽ� ���μ����� �����ٰ� �޽����� ���� ��
		//		break;

		case LTM_DELAY :	
		  for(int i=0;i<16;i++){		
			  if(i%5==0)  	 debug_printf("\r\n");		 
			  debug_printf("%d:%7.2f\t",i,AI_Data.DFTvalue[i].mag);
		  }
		  debug_printf("\r\n");			
		  break;

		default :
			// status�� ���� �۾��� �����Ѵ�
			;
	}
}
