#include <string.h>		// memset
#include <stdlib.h>		// NULL, malloc

#include <string.h>
#include "sysmsg.h"
#include "timer.h"
#include "procmsg.h"

#include "getopt.h"
#include "debug.h"		// callback �Լ� ����� ���Ͽ�
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

	// �θ� �����Ѵ�
	callbackmsg_t parentf;

	callbackf_tranceive recvf;
	callbackf_tranceive sendf;
	// �ڽĿ� ���� ���� ���̽�
}  CmdSetMem_t, *PCmdSetMem_t;
void memset_DispatchMessage(int32_t msg, int32_t params);
void memset_Entry(int argc, char *argv[]);
COMMAND cmd_memset = { "memset", memset_Entry, 0 };
void memset_Exit(void)
{
	PCmdSetMem_t pCmd = (PCmdSetMem_t)pDynamicCmdVariable;

	// �θ� �����Ѵ�
	debug_setcallbackf(pCmd->parentf);

	// �θ𿡰� �۾��� �������� �˸���
	SendMessage(pCmd->parentf, SM_CHILD_CLOSED, 0, 9);

#ifdef	DEBUG_MSG
	debug_printf("memset: free var=0x%p\r\n", pCmd);
#endif

	// heap ������ �Ҵ�� ������ �����
	if ( pCmd ) free(pCmd);
}


uint8_t testnum=0;

uint16_t TEST_SRAM2[5]={0,};
uint64_t test_flt=0;
void memset_Entry(int argc, char *argv[])
{

/* �ݵ�� ���� �� �κ� */
	// �ʿ��� ������ heap ������ �Ҵ��Ѵ�
	PCmdSetMem_t pCmd = (PCmdSetMem_t)malloc(sizeof(CmdSetMem_t));

#ifdef	DEBUaG_MSG
	debug_printf("memset: malloc var=0x%p\r\n", pCmd);
#endif

	// Dynamic ������ ���� ������ �����Ѵ�
	pDynamicCmdVariable = pCmd;		// defined at command.h

	// �Ҵ�� ������ ��� �ʱ�ȭ �Ѵ�
	if ( pCmd )	memset(pCmd, 0, sizeof(CmdSetMem_t) );

	// �θ� �����Ѵ�
	pCmd->parentf = debug_getcallbackf();

	// �ۼ��� �̺�Ʈ�� �� �Լ��� �����Ѵ�
	debug_setcallbackf(memset_DispatchMessage);

	// memset ��ɾ�� � �ɼǵ��� �ִ°�?
	// ������ �޸𸮿� �����͸� ������ �����Ѵ�
	// -s start address
	// -d device id
	// -t type 32bit 16bit 8bit
	// memset -s 0x1234 0x34 0x
	// ������ �˻��Ѵ�

				

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
		
	// �غ� �Ϸ� �Ǿ����� �˸���
			SendMessage(memset_DispatchMessage, SM_INITIALIZED, 0, 9);
}

void memset_DispatchMessage(int32_t msg, int32_t params)
{
	switch( msg )
	{
		case SM_RECEIVED ://  cnt
			SendMessage(memset_DispatchMessage, SM_CLOSED, 0, 9);;
			// recv �Լ��� �̿��Ͽ� �����͸� �о�´�
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
		// �ڽ� ���μ����� �����ٰ� �޽����� ���� ��
		//		break;

		default :
			// status�� ���� �۾��� �����Ѵ�
			;
	}
}

