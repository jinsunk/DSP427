#include "sysmsg.h"
#include "procmsg.h"


#include "console.h"

#define	DEBUG_MSG

#include <stdio.h>			// for printf, vsprintf
#include <stdarg.h>			// va_list

#define	CONSOLE_TX_BUFF_SIZE	2048	// 2�� �¼��� �������� ��
#define	CONSOLE_RX_BUFF_SIZE	256		// 2, 4, 8, 16, 32, 64, 128, 256

#define	ST_CONSOLE_READY	(ST_LOCAL+1)

static uint8_t _con_rx_buff[CONSOLE_RX_BUFF_SIZE];
static uint8_t _con_tx_buff[CONSOLE_TX_BUFF_SIZE];

static uint8_t *_con_rxrd_ptr, *_con_rxwr_ptr;
static uint8_t *_con_txrd_ptr, *_con_txwr_ptr;

/* local static variables */
static int32_t _con_rxlen;
static int32_t _console_status;
static callbackmsg_t _console_transfer_f;

static char console_msg[256];

extern void console_DispatchMessage(int32_t msg, int32_t params);

void console_RegisterMsgHandle(callbackmsg_t newf)
{
	_console_transfer_f = newf;
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

		SendSystemMessage(_console_transfer_f, SM_RECEIVED, _con_rxlen);	// sysmsg.h
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

			SendSystemMessage(_console_transfer_f, SM_TRANSMITTED, 0);	// sysmsg.h
		}
	}
}

void console_Init(void)
{
	// �ʱ�ȭ
	_con_rxrd_ptr = _con_rxwr_ptr = _con_rx_buff;
	_con_txrd_ptr = _con_txwr_ptr = _con_tx_buff;

//	board_ConsoleSetSW();

	// default dispatch function
	_console_transfer_f = console_DispatchSysMsg;

	// console�� ���Ͽ� �۾��� �� �ִ� command�� �����Ѵ�
//	ListInit(&_lDebugCommand, ASCENT|LINEAR);


	// command�� �ش�Ǵ� instance�� �����

/*
	// ADD Command List
	PNODE pNode;

	pNode = (PNODE)malloc(sizeof(NODE));
	pNode->pObject = &cmd_Help;
	ListAddNodeTail(&_lDebugCommand, pNode);

	pNode = (PNODE)malloc(sizeof(NODE));
	pNode->pObject = &cmd_SetPhy;
	ListAddNodeTail(&_lDebugCommand, pNode);

	pNode = (PNODE)malloc(sizeof(NODE));
	pNode->pObject = &cmd_GetPhy;
	ListAddNodeTail(&_lDebugCommand, pNode);

	pNode = (PNODE)malloc(sizeof(NODE));
	pNode->pObject = &cmd_ReadPhy;
	ListAddNodeTail(&_lDebugCommand, pNode);
*/
	// ���ο� ��带 �����
//	pNode = (PNODE)malloc(sizeof(NODE));
//	pNode->pObject = &cmd_ReadPhy;
//	���� ���� ���� �Լ��� ������
//	���� ������ transfer �Լ��� ������

}


// ------------------------------------------------------
// interface functions
// ------------------------------------------------------
int32_t console_Send(char *pbuff, int32_t size)
{
#ifdef	DEBUG
	if ( _console_status != ST_CONSOLE_READY )
	{
		pbuff[size] = '\0';

		printf("E %s", pbuff);
		return 0;
	}
#endif

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


int32_t console_printf(const char *format, ...)
{
	int32_t rstval;
	va_list ap;

	va_start(ap, format);
	rstval = vsprintf(console_msg, format, ap);
	va_end(ap);

	if ( rstval > 0 )
		console_Send(console_msg, rstval);

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

