#include "sysmsg.h"
#include "procmsg.h"


#include "console.h"

#define	DEBUG_MSG

#include <stdio.h>			// for printf, vsprintf
#include <stdarg.h>			// va_list

#define	CONSOLE_TX_BUFF_SIZE	2048	// 2의 승수로 지정해줄 것
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
		// 수신된 데이터를 링 버퍼에 넣는다
		*_con_rxwr_ptr++ = CONPORT->DR;
		_con_rxlen++;

		if ( _con_rxwr_ptr == (_con_rx_buff + CONSOLE_RX_BUFF_SIZE) )
			_con_rxwr_ptr = _con_rx_buff;
	}

	if ( srr & USART_FLAG_IDLE )
	{
		CONPORT->DR;	// 읽어서 버린다(to clear USART_FLAG_IDLE bit in SR)

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
			// TX interrupt를 disable한다
			CONPORT->CR1 &= ~USART_FLAG_TXE;

			SendSystemMessage(_console_transfer_f, SM_TRANSMITTED, 0);	// sysmsg.h
		}
	}
}

void console_Init(void)
{
	// 초기화
	_con_rxrd_ptr = _con_rxwr_ptr = _con_rx_buff;
	_con_txrd_ptr = _con_txwr_ptr = _con_tx_buff;

//	board_ConsoleSetSW();

	// default dispatch function
	_console_transfer_f = console_DispatchSysMsg;

	// console을 통하여 작업할 수 있는 command를 정의한다
//	ListInit(&_lDebugCommand, ASCENT|LINEAR);


	// command에 해당되는 instance를 만든다

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
	// 새로운 노드를 만든다
//	pNode = (PNODE)malloc(sizeof(NODE));
//	pNode->pObject = &cmd_ReadPhy;
//	내가 응답 받을 함수의 포인터
//	현재 지정된 transfer 함수의 포인터

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

	// TX interrupt를 enable한다
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

// 현재까지 수신된 데이터의 길이를 리턴한다
int32_t console_RecvSize(void)
{
	int32_t recv_size = (((uint32_t)_con_rxwr_ptr - (uint32_t)_con_rxrd_ptr) & (uint32_t)(CONSOLE_RX_BUFF_SIZE - 1));

	return recv_size;
}


int32_t console_Recv(uint8_t *pbuff, int32_t maxsize)
{
	int32_t ret_size;
	int32_t recv_size = console_RecvSize();

	// 링버퍼에서 선형버퍼로 copy한다
	// 선형 버퍼의 사이즈를 초과하지 않게 주의한다
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

