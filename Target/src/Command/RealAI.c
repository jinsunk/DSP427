#include <string.h>		// memset
#include <stdlib.h>		// NULL, malloc

#include "sysmsg.h"
#include "timer.h"
#include "procmsg.h"

#include "getopt.h"
#include "debug.h"		// callback 함수 등록을 위하여
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
	// 부모를 저장한다
	callbackmsg_t parentf;

	callbackf_tranceive recvf;
	callbackf_tranceive sendf;
	// 자식에 대한 변수 베이스
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

	// 부모를 복원한다
	debug_setcallbackf(pCmd->parentf);
	
		  debug__setcallbackf(pCmd->parentf);
	// 부모에게 작업이 끝났음을 알린다
	SendMessage(pCmd->parentf, SM_CHILD_CLOSED, 0, 9);

#ifdef	DEBUG_MSG
	debug_printf("Real Analog Input: free var=0x%p\r\n", pCmd);
#endif

	// heap 영역에 할당된 변수를 지운다
	if ( pCmd ) free(pCmd);
}

void RealAI_Entry(int argc, char *argv[])
{
/* 반드시 들어가야 할 부분 */
	// 필요한 변수를 heap 영역에 할당한다
	PCmdSetAI_t pCmd = (PCmdSetAI_t)malloc(sizeof(CmdSetAI_t));

#ifdef	DEBUG_MSG
	debug_printf("Real Analog Input: malloc var=0x%p\r\n", pCmd);
#endif

	// Dynamic 변수의 시작 번지를 보관한다
	pDynamicCmdVariable = pCmd;		// defined at command.h

	// 할당된 변수를 모두 초기화 한다
	if ( pCmd )	memset(pCmd, 0, sizeof(CmdSetAI_t) );

	// 부모를 저장한다
	pCmd->parentf = debug_getcallbackf();

	// 송수신 이벤트를 이 함수를 지정한다
	debug_setcallbackf(RealAI_DispatchMessage);
	debug__setcallbackf(RealAI_DispatchMessage);
	// memset 명령어는 어떤 옵션들이 있는가?
	// 지정된 메모리에 데이터를 강제로 설정한다


	debug_printf("Analog Input \n\r");

	
	//debug_printf("\n");
	

	// 준비 완료 되었음을 알린다
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
			// 다음 송신을 위하여
			// 다음 송신이 없으면 끝낸다
			// 다음 송신이 있으면 
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
		// 자식 프로세스가 끝났다고 메시지가 왔을 때
		//		break;

		case LTM_DELAY :	
		  for(int i=0;i<16;i++){		
			  if(i%5==0)  	 debug_printf("\r\n");		 
			  debug_printf("%d:%7.2f\t",i,AI_Data.DFTvalue[i].mag);
		  }
		  debug_printf("\r\n");			
		  break;

		default :
			// status에 따라 작업을 진행한다
			;
	}
}
