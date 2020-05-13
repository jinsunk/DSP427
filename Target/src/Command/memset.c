#include <string.h>		// memset
#include <stdlib.h>		// NULL, malloc

#include <string.h>
#include "sysmsg.h"
#include "timer.h"
#include "procmsg.h"

#include "getopt.h"
#include "debug.h"		// callback 함수 등록을 위하여
#include "Define.h" 

#include "board.h"
#include "command.h"

extern uint16_t op_buffnum;
// #define	DEBUG_MSG
typedef struct {
	int32_t mode;
	int32_t status;

	int32_t argc;
	// char *argvf[];

	// 부모를 저장한다
	callbackmsg_t parentf;

	callbackf_tranceive recvf;
	callbackf_tranceive sendf;
	// 자식에 대한 변수 베이스
}  CmdSetMem_t, *PCmdSetMem_t;
void memset_DispatchMessage(int32_t msg, int32_t params);
void memset_Entry(int argc, char *argv[]);
COMMAND cmd_memset = { "memset", memset_Entry, 0 };
void memset_Exit(void)
{
	PCmdSetMem_t pCmd = (PCmdSetMem_t)pDynamicCmdVariable;

	// 부모를 복원한다
	debug_setcallbackf(pCmd->parentf);

	// 부모에게 작업이 끝났음을 알린다
	SendMessage(pCmd->parentf, SM_CHILD_CLOSED, 0, 9);

#ifdef	DEBUG_MSG
	debug_printf("memset: free var=0x%p\r\n", pCmd);
#endif

	// heap 영역에 할당된 변수를 지운다
	if ( pCmd ) free(pCmd);
}


uint8_t testnum=0;

uint16_t TEST_SRAM2[5]={0,};
uint64_t test_flt=0;
void memset_Entry(int argc, char *argv[])
{

/* 반드시 들어가야 할 부분 */
	// 필요한 변수를 heap 영역에 할당한다
	PCmdSetMem_t pCmd = (PCmdSetMem_t)malloc(sizeof(CmdSetMem_t));

#ifdef	DEBUaG_MSG
	debug_printf("memset: malloc var=0x%p\r\n", pCmd);
#endif

	// Dynamic 변수의 시작 번지를 보관한다
	pDynamicCmdVariable = pCmd;		// defined at command.h

	// 할당된 변수를 모두 초기화 한다
	if ( pCmd )	memset(pCmd, 0, sizeof(CmdSetMem_t) );

	// 부모를 저장한다
	pCmd->parentf = debug_getcallbackf();

	// 송수신 이벤트를 이 함수를 지정한다
	debug_setcallbackf(memset_DispatchMessage);

	// memset 명령어는 어떤 옵션들이 있는가?
	// 지정된 메모리에 데이터를 강제로 설정한다
	// -s start address
	// -d device id
	// -t type 32bit 16bit 8bit
	// memset -s 0x1234 0x34 0x
	// 문법을 검사한다

				

	int c;
	while( (c = getopt(argc, argv, "memset :s :d :h")) != -1)
	{
		// -1 means getopt() parse all options
		switch(c) {
	  
			case 's':			
				event_Oper(0x1011,op_buffnum);
				
				break;

			case 'd':
	
			for(int i=0;i<5;i++){
			TEST_SRAM2[i]=*(uint16_t *)(SDRAM_OPER_ADDR+(5*2*testnum)+(i*2));
			}
			debug_printf("%x,%x, %x,%x,%x",TEST_SRAM2[0],TEST_SRAM2[1],TEST_SRAM2[2],TEST_SRAM2[3],TEST_SRAM2[4]);
	            break;

			case 'h':			
				event_FLT(test_flt);
				break;
			default :			
				break;
			
		}

	}
		
	// 준비 완료 되었음을 알린다
			SendMessage(memset_DispatchMessage, SM_INITIALIZED, 0, 9);
}

void memset_DispatchMessage(int32_t msg, int32_t params)
{
	switch( msg )
	{
		case SM_RECEIVED ://  cnt
			SendMessage(memset_DispatchMessage, SM_CLOSED, 0, 9);;
			// recv 함수를 이용하여 데이터를 읽어온다
			break;

		case SM_TRANSMITTED :		
			//SendMessage(memset_DispatchMessage, SM_INITIALIZED, 0, 9);
			break;

		case SM_INITIALIZED :
			SendMessage(memset_DispatchMessage, SM_CLOSED, 0, 9);;
			break;

		case SM_CLOSED :
			memset_Exit();
			break;

		//		case SM_CHILD_CLOSED :
		// 자식 프로세스가 끝났다고 메시지가 왔을 때
		//		break;

		default :
			// status에 따라 작업을 진행한다
			;
	}
}

