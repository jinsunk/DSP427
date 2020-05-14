#include "sysmsg.h"
#include "procmsg.h"

#include "console.h"
#include "board.h"

#define	DEBUG_MSG

#include <stdio.h>		// for printf, vsprintf
#include <stdarg.h>		// va_list
#include <string.h>		// memset, strtok
#include <stdlib.h>		// malloc

#define	CONSOLE_TX_BUFF_SIZE	2048	// 2의 승수로 지정해줄 것
#define	CONSOLE_RX_BUFF_SIZE	256		// 2, 4, 8, 16, 32, 64, 128, 256

#define	ST_CONSOLE_READY	(ST_LOCAL+1)

static uint8_t _con_rx_buff[CONSOLE_RX_BUFF_SIZE];
static uint8_t _con_tx_buff[CONSOLE_TX_BUFF_SIZE];

static uint8_t *_con_rxrd_ptr, *_con_rxwr_ptr;
static uint8_t *_con_txrd_ptr, *_con_txwr_ptr;

/* local static variables */
static int32_t _con_rxlen;
static callbackmsg_t _console_transfer_f;

static LIST lCommand;

void console_DispatchSysMsg(int32_t sysmsg, int32_t params);
// extern void console_DispatchMessage(int32_t msg, int32_t params);

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
			// TX interrupt를 disable한다
			CONPORT->CR1 &= ~USART_FLAG_TXE;

			SendSystemMessage(console_DispatchSysMsg, SM_TRANSMITTED, 0);	// sysmsg.h
		}
	}
}

void console_ProcessInit(void)
{
	// 초기화
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
	_console_transfer_f = (void*)0;

	// 사용할 인라인 명령어를 등록한다
	ListInit(&lCommand, ASCENT | LINEAR);

/*
	// ADD Command List
	PNODE pNode;

	pNode = (PNODE)malloc(sizeof(NODE));
	pNode->pObject = &cmd_Help;
	ListAddNodeTail(&_lDebugCommand, pNode);

*/
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

	// TX interrupt를 enable한다
	CONPORT->CR1 |= USART_FLAG_TXE;

	return 0;
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

void console_DispatchSysMsg(int32_t sysmsg, int32_t params)
{
	if ( _console_transfer_f )
		SendMessage(_console_transfer_f, sysmsg, params, 9);
	else
	{
		switch( sysmsg )
		{
			case SM_RECEIVED :
				// 수신된 내용을 읽어온다
				// console_Recv(temp_buff32, params);
				break;

			case SM_CHILD_CLOSED :
				// console_Send("\r\n", 2);
				// console_Send(_str_prefix, strlen(_str_prefix));
				break;

			default :
				;
		}

	}
}

