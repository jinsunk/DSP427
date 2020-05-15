#include "sysmsg.h"
#include "procmsg.h"

#include "console.h"
#include "board.h"
#include "mystring.h"
#include "getopt.h"

// #define	DEBUG_MSG

#include <stdio.h>		// for printf, vsprintf
#include <stdarg.h>		// va_list
#include <string.h>		// memset, strtok
#include <stdlib.h>		// malloc

#define	CONSOLE_TX_BUFF_SIZE	2048	// 2�� �¼��� �������� ��
#define	CONSOLE_RX_BUFF_SIZE	256		// 2, 4, 8, 16, 32, 64, 128, 256

#define	ST_CONSOLE_READY	(ST_LOCAL+1)

static uint8_t _con_rx_buff[CONSOLE_RX_BUFF_SIZE];
static uint8_t _con_tx_buff[CONSOLE_TX_BUFF_SIZE];

static uint8_t *_con_rxrd_ptr, *_con_rxwr_ptr;
static uint8_t *_con_txrd_ptr, *_con_txwr_ptr;

/* local static variables */
static int32_t _con_rxlen;
static callbackmsg_t _console_transfer_f;


// ----------------------------------------------------------------------------
// Inline command ���� ���� ����
void *pDynamicCmdVariable;
static LIST lCommand;
static CMDSTRING _str_command;
static char _str_prefix[4] = "# ";



void help_Entry(int argc, char *argv[]);
void help_DispatchMessage(int32_t msg, int32_t params);
void help_Exit(void);


static COMMAND cmd_HELP = {
	"help",
	help_Entry,
//	0
};

typedef struct {
	int32_t mode;
//	int32_t status;

//	int32_t argc;
	// char *argvf[];

	PNODE pNode;	// command�� Node�� �����ϱ� ���� ����Ѵ�

	// �θ� ����
	callbackmsg_t parentf;

	// callbackf_tranceive recvf;
	// callbackf_tranceive sendf;

	// �ڽĿ� ���� ���� ���̽�
}  CmdHelp_t, *PCmdHelp_t;


void console_DispatchSysMsg(int32_t sysmsg, int32_t params);
void console_DispatchMessage(int32_t msg, int32_t params);

void console_RegisterMsgHandle(callbackmsg_t newf)
{
	_console_transfer_f = newf;
}

callbackmsg_t console_GetMsgHandle(void)
{
	return _console_transfer_f;
}

// for STM32F4xx CONPORT is define at board.h
void CONPORT_HANDLER(void)
{
	uint16_t srr;

	srr = CONPORT->SR;

	// check if the CONPORT receive interrupt flag was set
	if ( srr & USART_FLAG_RXNE )
	{
		// ���ŵ� �����͸� �� ���ۿ� �ִ´�
		*_con_rxwr_ptr++ = CONPORT->DR;
		_con_rxlen++;

		if ( _con_rxwr_ptr == (_con_rx_buff + CONSOLE_RX_BUFF_SIZE) )
			_con_rxwr_ptr = _con_rx_buff;
	}

	if ( srr & USART_FLAG_IDLE )
	{
		CONPORT->DR;	// �о ������(to clear USART_FLAG_IDLE bit in SR)

		SendSystemMessage(console_DispatchSysMsg, SM_RECEIVED, _con_rxlen);	// sysmsg.h
		_con_rxlen = 0;
	}

	if ( (CONPORT->CR1 & USART_FLAG_TXE) && (srr & USART_FLAG_TXE) )
	{
		if ( _con_txwr_ptr != _con_txrd_ptr )
		{
			CONPORT->DR = *_con_txrd_ptr++;

			if ( _con_txrd_ptr == (_con_tx_buff + CONSOLE_TX_BUFF_SIZE) )
				_con_txrd_ptr = _con_tx_buff;
		}
		else
		{
			// TX interrupt�� disable�Ѵ�
			CONPORT->CR1 &= ~USART_FLAG_TXE;

			SendSystemMessage(console_DispatchSysMsg, SM_TRANSMITTED, 0);	// sysmsg.h
		}
	}
}

void console_ProcessInit(void)
{
	// �ʱ�ȭ
	_con_rxrd_ptr = _con_rxwr_ptr = _con_rx_buff;
	_con_txrd_ptr = _con_txwr_ptr = _con_tx_buff;

	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable RXNE interrupt */
	USART_ITConfig(CONPORT, USART_IT_RXNE, ENABLE);
	USART_ITConfig(CONPORT, USART_IT_IDLE, ENABLE);

	/* Enable USART1 global interrupt */
//	NVIC_EnableIRQ(CONPORT_IRQ);

	/* Enable the TIM5 global Interrupt for counting seconds */
	NVIC_InitStructure.NVIC_IRQChannel = CONPORT_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// default dispatch function
	_console_transfer_f = console_DispatchMessage;

	memset(&_str_command, 0, sizeof(CMDSTRING));
	_str_command.echo = CMDMODE_NORMAL;

	// ����� �ζ��� ��ɾ �ʱ�ȭ�Ѵ�
	ListInit(&lCommand, ASCENT | LINEAR);

	// �⺻������ help ��ɾ �߰��Ѵ�
	console_AddCommand(&lCommand, &cmd_HELP);

	// help ��ɾ �ִٰ� console�� �˷� �ش�
	console_printf("type 'help' for list of usable commands\r\n");
}

// ------------------------------------------------------
// interface functions
// ------------------------------------------------------
int32_t console_Send(char *pbuff, int32_t size)
{
	while( size-- )
	{
		*_con_txwr_ptr++ = *pbuff++;

		if ( _con_txwr_ptr == (_con_tx_buff + CONSOLE_TX_BUFF_SIZE) )
			_con_txwr_ptr = _con_tx_buff;
	}

	// TX interrupt�� enable�Ѵ�
	CONPORT->CR1 |= USART_FLAG_TXE;

	return 0;
}

int32_t console_errormsg(void *vmsg)
{
	uint8_t vt100b[8] = { 0x1b, '[', '3', '7', ';', '4', '1', 'm' };
	uint8_t vt100e[4] = { 0x1b, '[', '0', 'm' };
	uint8_t vt100l[4] = { '\r', '\n' };

	// ���� ����������
	uint8_t *msg = vt100b;
	int32_t nSize = 8;

	while( nSize-- )
	{
		*_con_txwr_ptr++ = *msg++;

		if ( _con_txwr_ptr == (_con_tx_buff + CONSOLE_TX_BUFF_SIZE) )
			_con_txwr_ptr = _con_tx_buff;
	}

	// error message�� ���
	msg = (uint8_t*)vmsg;
	nSize = strlen((char const*)msg);

	while( nSize-- )
	{
		*_con_txwr_ptr++ = *msg++;

		if ( _con_txwr_ptr == (_con_tx_buff + CONSOLE_TX_BUFF_SIZE) )
			_con_txwr_ptr = _con_tx_buff;
	}

	// ���� �������
	msg = vt100e;
	nSize = 4;

	while( nSize-- )
	{
		*_con_txwr_ptr++ = *msg++;

		if ( _con_txwr_ptr == (_con_tx_buff + CONSOLE_TX_BUFF_SIZE) )
			_con_txwr_ptr = _con_tx_buff;
	}

	// �ٹٲ� ���
	msg = vt100l;
	nSize = 2;

	while( nSize-- )
	{
		*_con_txwr_ptr++ = *msg++;

		if ( _con_txwr_ptr == (_con_tx_buff + CONSOLE_TX_BUFF_SIZE) )
			_con_txwr_ptr = _con_tx_buff;
	}

	// TX interrupt�� enable�Ѵ�
	CONPORT->CR1 |= USART_FLAG_TXE;

	return nSize;
}

int32_t console_printf(const char *format, ...)
{
	int32_t rstval;
	va_list ap;
	char msg[256];

	va_start(ap, format);
	rstval = vsprintf(msg, format, ap);
	va_end(ap);

	if ( rstval > 0 )
		console_Send(msg, rstval);

	return rstval;
}

// ������� ���ŵ� �������� ���̸� �����Ѵ�
int32_t console_RecvSize(void)
{
	int32_t recv_size = (((uint32_t)_con_rxwr_ptr - (uint32_t)_con_rxrd_ptr) & (uint32_t)(CONSOLE_RX_BUFF_SIZE - 1));

	return recv_size;
}


int32_t console_Recv(uint8_t *pbuff, int32_t maxsize)
{
	int32_t ret_size;
	int32_t recv_size = console_RecvSize();

	// �����ۿ��� �������۷� copy�Ѵ�
	// ���� ������ ����� �ʰ����� �ʰ� �����Ѵ�
	if ( recv_size > maxsize )
		ret_size = maxsize;
	else
		ret_size = recv_size;

	for( recv_size = 0; recv_size < ret_size; recv_size++)
	{
		*pbuff++ = *_con_rxrd_ptr++;
		if ( _con_rxrd_ptr == (_con_rx_buff + CONSOLE_RX_BUFF_SIZE) )
			_con_rxrd_ptr = _con_rx_buff;
	}

	return ret_size;
}

PCOMMAND console_FindCommand(PLIST pList, char *strcmd)
{
	PNODE pNode = pList->m_pNodeHead;
	PCOMMAND pCommand;

	while( pNode )
	{
		pCommand = (PCOMMAND)pNode->pObject;

		if ( strcmp(pCommand->cmdstr, strcmd) == 0 )
		{
			// ã���� ��
			return pCommand;
		}
		else
			pNode = pNode->pNext;
	}

	return (void*)0;	// �� ã���� ��
}

int console_AddCommand(PLIST pList, PCOMMAND pCommand)
{
	PNODE pNewNode;

	// NODE�� �����
	pNewNode = (PNODE)malloc(sizeof(NODE));

	if ( pNewNode )
	{
		pNewNode->pObject = pCommand;
		ListAddNodeTail(pList, pNewNode);
	}

	return 0;
}

void console_DispatchSysMsg(int32_t sysmsg, int32_t params)
{
	if ( _console_transfer_f )
		SendMessage(_console_transfer_f, sysmsg, params, 9);
}

void console_DispatchMessage(int32_t msg, int32_t params)
{
	uint8_t temp_buff[16];

	switch ( msg )
	{
		case SM_RECEIVED :
			// ���ŵ� ������ �о�´�
			while ( console_RecvSize() )
			{
				params = console_Recv(temp_buff, 16);
				switch( input_strcmd(&_str_command, params, (char*)temp_buff) )
				{
					case COMMAND_ENTER :
						if ( _str_command.length )
						{
							int argc;
							char *argv[16];

							if ( _str_command.ret_len )
								console_Send(_str_command.ret_str, _str_command.ret_len);

							// command list���� �Էµ� ���ڿ��� �ִ°� �˻��Ѵ�
							argc = getArguements(argv, _str_command.buff);
							if ( argc && argv[0] )
							{
								// command list���� _argv[0]�� �ش�Ǵ� ��ɾ ã�´�
								PCOMMAND pCommand = console_FindCommand(&lCommand, argv[0]);

								if ( pCommand )
								{	// ã���� ��
									// entry �Լ��� �����Ѵ�
									pCommand->entryf(argc, argv);
								}
								else
								{	// �� ã���� ��
									console_printf("\r\ncommand not found\r\n");
									console_Send(_str_prefix, strlen(_str_prefix));
								}
							}

							// ������ �����͸� �����
							clear_strcmd(&_str_command);
						}
						else
						{	// �ƹ��� ���ڰ� ���� ���ϸ� ���� ���� ��
							// prefix ���ڸ� ǥ���Ѵ�
							console_Send("\r\n", 2);
							console_Send(_str_prefix, strlen(_str_prefix));
						}
						break;

					case COMMAND_CANCEL :
						// ������ �����͸� �����
						clear_strcmd(&_str_command);

						// �����ٷ� �ѱ��
						console_Send("\r\n", 2);
						console_Send(_str_prefix, strlen(_str_prefix));
						break;

					case COMMAND_PREVIOUS :
						//
						break;

					case COMMAND_NEXT :
						break;

					default :
						// �۽��� �����Ͱ� �ִٸ� �۽��Ѵ�
						if ( _str_command.ret_len )
							console_Send(_str_command.ret_str, _str_command.ret_len);
				}
			}
			break;

		case SM_TRANSMITTED :
			break;

		case SM_CHILD_CLOSED :
			console_Send("\r\n", 2);
			console_Send(_str_prefix, strlen(_str_prefix));
			break;

		default :
			;
	}
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// help command functions
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
void help_Entry(int argc, char *argv[])
{
	/* �ݵ�� ���� �� �κ� */
	// �ʿ��� ������ heap ������ �Ҵ��Ѵ�
	PCmdHelp_t pCmd = (PCmdHelp_t)malloc(sizeof(CmdHelp_t));

#ifdef	DEBUG_MSG
//	console_printf("memset: malloc var=0x%p\r\n", pCmd);
#endif

	// Dynamic ������ ���� ������ �����Ѵ�
	pDynamicCmdVariable = pCmd;

	// �Ҵ�� ������ ��� �ʱ�ȭ �Ѵ�
	if ( pCmd )	memset(pCmd, 0, sizeof(CmdHelp_t) );

	// �θ� �����Ѵ�
	pCmd->parentf = console_GetMsgHandle();

	// �ۼ��� �̺�Ʈ�� �� �Լ��� �����Ѵ�
	console_RegisterMsgHandle(help_DispatchMessage);

	pCmd->mode = 1;

	int c;

	while( (c = getopt(argc, argv, "h")) != -1)
	{
		// -1 means getopt() parse all options
		switch(c) {
			case 'h':
				console_printf("help - show all usable commands\r\n");
				pCmd->mode = 0;
				break;

			default :
				;
		}
	}

	if ( pCmd->mode )
	{
		pCmd->pNode = lCommand.m_pNodeHead;

		console_printf("commands\r\n");

		SendMessage(help_DispatchMessage, SM_INITIALIZED, 0, 9);
	}
	else	// -h option�� ���� ��  �۾� ������ ����
		SendMessage(help_DispatchMessage, SM_CLOSED, 0, 9);

}

void help_Exit(void)
{
	PCmdHelp_t pCmd = (PCmdHelp_t)pDynamicCmdVariable;

	// �θ� �����Ѵ�
	console_RegisterMsgHandle(pCmd->parentf);

	// �θ𿡰� �۾��� �������� �˸���
	if ( pCmd->parentf )
		SendMessage(pCmd->parentf, SM_CHILD_CLOSED, 0, 9);

#ifdef	DEBUG_MSG
//	console_printf("help: free var=0x%p\r\n", pCmd);
#endif

	// heap ������ �Ҵ�� ������ �����
	if ( pCmd ) free(pCmd);
}

void help_DispatchMessage(int32_t msg, int32_t params)
{
	PCmdHelp_t pCmd = (PCmdHelp_t)pDynamicCmdVariable;

	switch( msg )
	{
		case SM_TRANSMITTED :
			pCmd->pNode = pCmd->pNode->pNext;
			SendMessage(help_DispatchMessage, SM_INITIALIZED, 0, 9);
			break;

		case SM_INITIALIZED :
			if ( pCmd->pNode )	// list�� ���� �����ϸ�
			{
				PCOMMAND pCommand = (PCOMMAND)pCmd->pNode->pObject;

				// console_printf("\t%s\r\n", pCommand->cmdstr);
				console_Send("\t",  1);
				console_Send(pCommand->cmdstr, strlen(pCommand->cmdstr));
				console_Send("\r\n",  2);
			}
			else
				SendMessage(help_DispatchMessage, SM_CLOSED, 0, 9);
			break;

		case SM_CLOSED :
			help_Exit();
			break;

		default :
			// status�� ���� �۾��� �����Ѵ�
			;
	}
}

// --------------------------------------------------------------------------
