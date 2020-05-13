#include <stdio.h>	// for FILE and printf
#include <time.h>

#include "board.h"

// debug message를 console에 표시하려면 DEBUG_MSG를 정의한다
#define	DEBUG_MSG

RCC_ClocksTypeDef RCC_Clocks;

/* ----------------------------------------------------
 *
 * System hardware control functions
 *
 * ---------------------------------------------------- */
/* stm32f4xx_conf.h에 정의 되어 있다 */
void assert_failed(uint8_t* file, uint32_t line)
{
	printf("syntax error. line #%d\r\nfile:%s\r\nsystem halt!\r\n", line, file);
	while(1);
}

/* 최소한의 H/W만 초기화한다 */
void _SysClock_Init(void)
{
	/* Configure pins as GPIO for Promet-FGTMU Communication Boards */
	/* Enable GPIOs clock */
	RCC_AHB1PeriphClockCmd(
		RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|
		RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|
		RCC_AHB1Periph_GPIOG|RCC_AHB1Periph_GPIOH|RCC_AHB1Periph_GPIOI, ENABLE);

	/* Enable CRC clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);

	// CRC->DR = 0xFFFFFFFF로 만든다
	CRC_ResetDR();

	/* Compute the CRC of "DataBuffer" */
	// uwCRCValue = CRC_CalcBlockCRC((uint32_t *)aDataBuffer, BUFFER_SIZE);

	/* Enable SYSCFG clock for remap the memory accessible in the code area, */
	/* select the Ethernet PHY interface and manage the external interrupt line */
	/* connection to the GPIOs*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Enable FMC clock */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);

	/* Enable the PWR clock to access RTC block */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Enable UART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* TIM1 clock enable */
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	/* TIM2 clock enable for System RTC */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* TIM3 clock enable for  */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* TIM5 clock enable for 1usec tick count */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	/* TIM8 clock enable for ADC Clock Control */
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

	/* Enable DMA clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
}


#define TEST_MCO

void _GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

/* ----------------------------------------------------------------------------------------------------------------------------- */
/*
	PA0 ------>	AIN(+12V)
	PA1 ------>	AIN(+5.2V)
	PA2 ------>	AIN(+3.3V)
	PA3	------> AIN(Vcore)
	PA4	------> Not Used
	PA5 ------>	Not Used
	PA6	------> Not Used
	PA7	------> Not Used
	PA8	------> MCO1	- Clock Test
	PA9 ------>	USART1 TXD
	PA10 -----> USART1 RXD
	PA11 -----> Not Used
	PA12 -----> Not Used
	PA13 -----> JTAG TMS
	PA14 -----> JTAG TCK
	PA15 -----> JTAG TDI
*/
	// Enable USART1
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_9|GPIO_Pin_10
#ifdef	TEST_MCO
		|GPIO_Pin_8
#endif
		);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		// GPIO Mode GPIO_Mode_IN, GPIO_Mode_OUT, GPIO_Mode_AF or GPIO_Mode_AN
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	// GPIO Output Maximum frequency 2, 25, 50 or 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		// GPIO Output Type GPIO_OType_PP or GPIO_OType_OD
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		// GPIO PullUp PullDown GPIO_PuPd_NOPULL, GPIO_PuPd_UP or GPIO_PuPd_DOWN

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Connect PXx to USARTx_Tx*/
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

#ifdef	TEST_MCO
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);
#endif

	// Enable Analog Input on ADC
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		// GPIO Mode GPIO_Mode_IN, GPIO_Mode_OUT, GPIO_Mode_AF or GPIO_Mode_AN
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	// GPIO Output Maximum frequency 2, 25, 50 or 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		// GPIO Output Type GPIO_OType_PP or GPIO_OType_OD
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	// GPIO PullUp PullDown GPIO_PuPd_NOPULL, GPIO_PuPd_UP or GPIO_PuPd_DOWN

	GPIO_Init(GPIOA, &GPIO_InitStructure);

/* ----------------------------------------------------------------------------------------------------------------------------- */
/*
	PB0	------>
	PB1	------>
	PB2	------>				(Boot1)
	PB3	------>	JTAG TDO
	PB4	------>	JTAG nTRST
	PB5	------>	CAN2 RX
	PB6	------>	CAN2 TX
	PB7	------>	Not Used
	PB8	------>	CAN1 RX
	PB9	------>	CAN1 TX
	PB10 -----> NC
	PB11 -----> NC
	PB12 -----> SPI_nSS(Input)
	PB13 -----> SPI2_SCK(AF)
	PB14 -----> SPI2_MISO(AF)
	PB15 -----> SPI2_MOSI(AF)
*/

	// Enable CANx
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_5|GPIO_Pin_6) | (GPIO_Pin_8|GPIO_Pin_9);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		// GPIO Output Type GPIO_OType_PP or GPIO_OType_OD
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Connect PXx to CAN1 */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_CAN1);

	/* Connect PXx to CAN2 */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_CAN2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_CAN2);

/* ----------------------------------------------------------------------------------------------------------------------------- */
/*
	PC0	------> S_Close
	PC1	------> S_OPEN
	PC2	------> TCS1 (S_CTS)
	PC3	------>
	PC4	------>
	PC5	------>
	PC6	------>
	PC7	------>	AF (Input Capture TIM3_CH2)
	PC8	------>	AF (Input Capture TIM3_CH3)
	PC9	------>	AF (MCO2	- Clock Test)
	PC10 ----->
	PC11 ----->
	PC12 ----->
	PC13 ----->	Pulse per Second. depend in time master or salve(input when boot)
	PC14 ----->	OSC32768 In
	PC15 ----->	OSC32768 Output
*/

	/* Configure PC7 and PC8 pin as input */
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_7
#ifdef TEST_MCO
		|GPIO_Pin_8
#endif
		);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure PC9 pin as AF */
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_9);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	// 2, 25, 50 or 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	// 2, 25, 50 or 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);



#ifdef TEST_MCO
	// RCC_MCO2Source_SYSCLK RCC_MCO2Source_PLLI2SCLK RCC_MCO2Source_HSE  RCC_MCO2Source_PLLCLK
	RCC_MCO2Config(RCC_MCO2Source_SYSCLK, RCC_MCO2Div_1);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_MCO);
#endif

	GPIO_Init(GPIOC, &GPIO_InitStructure);

/* ----------------------------------------------------------------------------------------------------------------------------- */
/*
	PD0	------> FMC Data2
	PD1	------> FMC Data3
	PD2	------> Input(ADC End interrupt)
	PD3	------>
	PD4	------> FMC nOE
	PD5	------> FMC nWE
	PD6	------> 		 (FMC nWAIT : 사용하면 안됨 Errta 2.9.2참고)
	PD7	------> FMC nCE2 (Nand Flash Chip Select)
	PD8	------> FMC Data13
	PD9	------> FMC Data14
	PD10 -----> FMC Data15
	PD11 -----> FMC NAND CLE
	PD12 -----> FMC NAND ALE
	PD13 ----->
	PD14 -----> FMC Data0
	PD15 -----> FMC Data1
*/

	/* Configure PD0, PD1, PD4, PD5, PD8, PD9, PD0, PD10, PD11, PD12, PD13, PD14 and PD15 pin as AF for FMC */
	GPIO_InitStructure.GPIO_Pin = (
		GPIO_Pin_0|GPIO_Pin_1|
		GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7|
		GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|
		GPIO_Pin_12|GPIO_Pin_14|GPIO_Pin_15);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	// 2, 25, 50 or 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FMC);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FMC);
//	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_FMC); 사용하면 안됨 Errta 2.9.2참고
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FMC);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FMC);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FMC);

	GPIO_Init(GPIOD, &GPIO_InitStructure);
/*
	//Configure PD2 pin as Input for External Interrupt
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_2);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

//	GPIO_Init(GPIOD, &GPIO_InitStructure);
*/
	// Connect External Interrupt pin to PD2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource2);

/* ----------------------------------------------------------------------------------------------------------------------------- */
/*
	PE0	 ------> FMC nBL0
	PE1	 ------> FMC nBL1
	PE2	 ------> Output (TCS nEN)
	PE3	 ------> Hardware Version 0
	PE4	 ------> Hardware Version 1
	PE5	 ------> Hardware Version 2
	PE6	 ------> Hardware Version 3
	PE7	 ------> FMC Data4
	PE8	 ------> FMC Data5
	PE9	 ------> FMC Data6
	PE10 ------> FMC Data7
	PE11 ------> FMC Data8
	PE12 ------> FMC Data9
	PE13 ------> FMC Data10
	PE14 ------> FMC Data11
	PE15 ------> FMC Data12
*/

	/* Configure PE0, PE1, PE7, PE8, PE9, PE10, PE11, PE12, PE13, PE14 and PE15 pin as AF for FMC */
	GPIO_InitStructure.GPIO_Pin = (
		GPIO_Pin_0|GPIO_Pin_1|
		GPIO_Pin_7|
		GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|
		GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	// 2, 25, 50 or 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource0, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF_FMC);

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FMC);

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FMC);

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FMC);

	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* Configure PE2, PE3 pin as Output for CAN Device Control */
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_2);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_WriteBit(GPIOF, GPIO_Pin_2, Bit_SET);		// 초기값 = 1

	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* Configure PE2, PE3 pin as Output for CAN Device Control */
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_Init(GPIOE, &GPIO_InitStructure);
/* ----------------------------------------------------------------------------------------------------------------------------- */
/*
	PF0	 ------> FMC A0
	PF1	 ------> FMC A1
	PF2	 ------> FMC A2
	PF3	 ------> FMC A3
	PF4	 ------> FMC A4
	PF5	 ------> FMC A5
	PF6	 ------>
	PF7	 ------>
	PF8	 ------> Output (nLedTest)
	PF9	 ------>
	PF10 ------>
	PF11 ------> FMC SDRAM nRAS
	PF12 ------> FMC A6
	PF13 ------> FMC A7
	PF14 ------> FMC A8
	PF15 ------> FMC A9
*/
	/* Configure PF0, PF1, PF2, PF3, PF4, PF5, PF11, PF12, PF13, PF14 and PF15 pin as AF for FMC */
	GPIO_InitStructure.GPIO_Pin = (
		GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
		GPIO_Pin_4|GPIO_Pin_5|
		GPIO_Pin_11|
		GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	// 2, 25, 50 or 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_PinAFConfig(GPIOF, GPIO_PinSource0, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource1, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource2, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource3, GPIO_AF_FMC);

	GPIO_PinAFConfig(GPIOF, GPIO_PinSource4, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource5, GPIO_AF_FMC);

	GPIO_PinAFConfig(GPIOF, GPIO_PinSource11, GPIO_AF_FMC);

	GPIO_PinAFConfig(GPIOF, GPIO_PinSource12, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource13, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource14, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource15, GPIO_AF_FMC);

	GPIO_Init(GPIOF, &GPIO_InitStructure);

	/* Configure PE7 pin as Output for Test Led Control */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	// GPIO_Write(GPIOF, 0);
	// GPIO_WriteBit(GPIOF, GPIO_Pin_7, Bit_SET);		// 초기값 = 1
	GPIO_Init(GPIOF, &GPIO_InitStructure);

/* ----------------------------------------------------------------------------------------------------------------------------- */
/*
	PG0	 ------> FMC A10
	PG1	 ------> FMC A11
	PG2	 ------> FMC A12
	PG3	 ------> FMC A13

	PG4	 ------> FMC BA0
	PG5	 ------> FMC BA1
	PG6	 ------> FMC nINT2
	PG7	 ------>

	PG8	 ------> FMC SDRAM CLK
	PG9	 ------> FMC nE2(AD7606 x 2)
	PG10 ------> FMC nE3(DualPort RAM)
	PG11 ------>

	PG12 ------>
	PG13 ------>
	PG14 ------>
	PG15 ------> FMC SDRAM nCAS
*/

	/* Configure PG0, PG1, PG2, PG3, PG4, PG5, PG6, PG8, PG9, PG10 and PG15 pin as AF for FMC */
	GPIO_InitStructure.GPIO_Pin = (
		GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
		GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|
		GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|
		GPIO_Pin_15);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	// 2, 25, 50 or 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_PinAFConfig(GPIOG, GPIO_PinSource0, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource1, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource2, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource3, GPIO_AF_FMC);

	GPIO_PinAFConfig(GPIOG, GPIO_PinSource4, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource5, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource6, GPIO_AF_FMC);	// FMC NAND Busy

	GPIO_PinAFConfig(GPIOG, GPIO_PinSource8, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource9, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource10, GPIO_AF_FMC);

	GPIO_PinAFConfig(GPIOG, GPIO_PinSource15, GPIO_AF_FMC);

	GPIO_Init(GPIOG, &GPIO_InitStructure);

/* ----------------------------------------------------------------------------------------------------------------------------- */
/*
	PH0	 ------> OSC 25MHz Clock In
	PH1	 ------>
	PH2	 ------>
	PH3	 ------>

	PH4	 ------>
	PH5	 ------> FMC SDRAM nWE
	PH6	 ------> FMC SDRAM nE1
	PH7	 ------> FMC SDRAM CKE1

	PH8	 ------>
	PH9	 ------>
	PH10 ------>
	PH11 ------>

	PH12 ------> Input (Dual Port Ram Interrupt)
	PH13 ------> Output (ADC Start)
	PH14 ------> Output (ADC Reset)
	PH15 ------> Output (NAND Write Protect nNF_WP)
*/
	/* Configure PH5, PH6 and PH7 pin as AF for FMC */
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	// 2, 25, 50 or 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_PinAFConfig(GPIOH, GPIO_PinSource5, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOH, GPIO_PinSource6, GPIO_AF_FMC);
	GPIO_PinAFConfig(GPIOH, GPIO_PinSource7, GPIO_AF_FMC);

	GPIO_Init(GPIOH, &GPIO_InitStructure);

	/* Configure PH13, PH14 and PH15 pin as Output */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	// 2, 25, 50 or 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_WriteBit(GPIOH, GPIO_Pin_13, Bit_RESET);		// ADC STOP
	GPIO_WriteBit(GPIOH, GPIO_Pin_14, Bit_SET);			// ADC RESET
	GPIO_WriteBit(GPIOH, GPIO_Pin_15, Bit_RESET);		// NAND FLASH WRITE PROTECTED

	GPIO_Init(GPIOH, &GPIO_InitStructure);

	/* Configure PH12 pin as Input for External Interrupt */
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_12);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_Init(GPIOH, &GPIO_InitStructure);

/* ----------------------------------------------------------------------------------------------------------------------------- */
/*	I Port는 인터럽트에서 Output을 control 하므로 다른 일반 DO와 같이 사용하지 말 것
	Input Port로 사용하는 것은 문제 없음

	PI0	 ------> Output(SPI2 nSS)
	PI1	 ------> SPI2 SCK
	PI2	 ------> SPI2 MISO
	PI3	 ------> SPI2 MOSI

	PI4	 ------> Output (Relay Trip Signal)
	PI5	 ------> Output (Relay Close Signal)
	PI6	 ------> Output (Relay System Fail Signal)
	PI7	 ------> Input (Trip Coil Supervisor Status 1)

	PI8	 ------> Input (Trip Coil Supervisor Status 2)
	PI9	 ------> Input (Trip Status)
	PI10 ------> Input (Close Status)
	PI11 ------>

	PI12 ------> (Not Exist on this Package)
	PI13 ------> (Not Exist on this Package)
	PI14 ------> (Not Exist on this Package)
	PI15 ------> (Not Exist on this Package)
*/
	/* Configure PI1, PI2 and PI3 pin as AF for SPI2 */
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	// 2, 25, 50 or 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

	// GPIO_PinAFConfig(GPIOI, GPIO_PinSource0, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource1, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource2, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource3, GPIO_AF_SPI2);

	GPIO_Init(GPIOI, &GPIO_InitStructure);

	/* Configure PI4, PI5 and PI6 pin as Output for Mechanical Relay */
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	// 2, 25, 50 or 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_WriteBit(GPIOI, GPIO_Pin_0, Bit_SET);		// SPI nSS = 1
	GPIO_WriteBit(GPIOI, GPIO_Pin_4, Bit_RESET);	// Trip = 0
	GPIO_WriteBit(GPIOI, GPIO_Pin_5, Bit_RESET);	// Close = 0
	GPIO_WriteBit(GPIOI, GPIO_Pin_6, Bit_RESET);	// SysFail = 0
	GPIO_Init(GPIOI, &GPIO_InitStructure);

	/* Configure PI7, PI8, PI9 and PI10 pin as Input for Solenoid Status */
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	// 2, 25, 50 or 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

	GPIO_Init(GPIOI, &GPIO_InitStructure);

/* -------------------------------------------------------------------------- */
}



void board_InterruptInit(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	//	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
	// Set vector table to 0x08020000 on internal Flash Memory
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x020000);

	// Nest Interrupt를 지원하는 방식을 선택한다
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the TIM2 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// NVIC_Init(&NVIC_InitStructure);

		/* TIM2 IT enable */
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

		/* Enable the TIM3 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// NVIC_Init(&NVIC_InitStructure);

		/* TIM3 IT enable */
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	// TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	/* Configure EXTI Line2 for ADS131E0x  */
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Line2 for ADS131E0x */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


}


/* ----------------------------------------------------
 *
 * UART Definitions
 *
 * ---------------------------------------------------- */
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */

	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}

	USART_SendData(USART1, (uint8_t)ch);

	return ch;
}

void _UART1_Init(void)
{
	USART_InitTypeDef USART_InitStructure;

	/* USART resources configuration (Clock, GPIO pins and USART registers) ----*/
	/* USART configured as follow:
		- BaudRate = 115200 baud
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate =115200;// 19200;//115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits =USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

	/* USART configuration */
	USART_Init(USART1, &USART_InitStructure);

	/* Enable USART */
	USART_Cmd(USART1, ENABLE);

}

/* ----------------------------------------------------
 *
 *  Interrupt Definitions
 *
 * ---------------------------------------------------- */
/**
  * @brief  setup all interrupt priorities
  * @param  None
  * @retval None
  */
/* ----------------------------------------------------
 *
 *  Timer Definitions
 *
 * ---------------------------------------------------- */
/**
  * @brief  setup all timers
  * @param  None
  * @retval None
  */
	/* -----------------------------------------------------------------------
	1/ Generate 3 complementary PWM signals with 3 different duty cycles:

	In this configuration TIM1 input clock (TIM1CLK) is set to 2 * APB2 clock (PCLK2),
	since APB2 prescaler is different from 1 (APB2 Prescaler = 2, see system_stm32f4xx.c file).
		TIM1CLK = 2 * PCLK2
		PCLK2 = HCLK / 2
		=> TIM1CLK = 2*(HCLK / 2) = HCLK = SystemCoreClock

	To get TIM1 counter clock at 180 MHz, the prescaler is computed as follows:
		Prescaler = (TIM1CLK / TIM1 counter clock) - 1
		Prescaler = (SystemCoreClock / 180 MHz) - 1 = 0

	The objective is to generate PWM signal at 17.57 KHz:
		- TIM1_Period = (SystemCoreClock / 17570) - 1

	To get TIM1 output clock at 17.57 KHz, the period (ARR) is computed as follows:
		ARR = (TIM1 counter clock / TIM1 output clock) - 1
			= 9561

	The Three Duty cycles are computed as the following description:
		TIM1 Channel1 duty cycle = (TIM1_CCR1/ TIM1_ARR)* 100 = 50%
		TIM1 Channel2 duty cycle = (TIM1_CCR2/ TIM1_ARR)* 100 = 25%
		TIM1 Channel3 duty cycle = (TIM1_CCR3/ TIM1_ARR)* 100 = 12.5%

	The Timer pulse is calculated as follows:
		- TIM1_CCRx = (DutyCycle * TIM1_ARR)/ 100

	2/ Insert a dead time equal to (11/SystemCoreClock) ns

	3/ Configure the break feature, active at High level, and using the automatic
		output enable feature

	4/ Use the Locking parameters level1.

	Note:
		SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
		Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
		function to update SystemCoreClock variable value. Otherwise, any configuration
		based on this variable will be incorrect.
	----------------------------------------------------------------------- */
void board_TimerInit(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	uint32_t TimerPeriod;

	RCC_GetClocksFreq(&RCC_Clocks);	// CLock 정보를 가져온다

	printf("HCLK   %dMHz\r\n", RCC_Clocks.HCLK_Frequency/1000000);
	printf("PCLK1   %dMHz\r\n", RCC_Clocks.PCLK1_Frequency/1000000);
	printf("PCLK2   %dMHz\r\n", RCC_Clocks.PCLK2_Frequency/1000000);
	printf("SYSCLK %dMHz\r\n", RCC_Clocks.SYSCLK_Frequency/1000000);


	// -----------------------------------------------
	// TIM2 32bit timer의 time tick에 사용할 timer 초기화
	// TIM2는 PCLK1을 2배수한 값이 입력된다
	// TIM_Prescaler값은 0xffff의 16bit 값이 최대이다
	// 그러므로 최선의 선택은 TIM2를 1msec로 돌리면 좋겠지만
	// PCLK1_Frequency =  45Mhz @ STM32F427 //  36Mhz @STM32F373
	// -----------------------------------------------
	/* Compute the value to be set in ARR register to generate signal frequency at 10kHz */
	TimerPeriod = ( (RCC_Clocks.PCLK1_Frequency*2) / 10000 ) - 1;

	TIM_TimeBaseStructure.TIM_Prescaler = TimerPeriod;	// 100uec;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 0xffffffff;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	/* TIM2 counter enable */

	TIM_Cmd(TIM2, ENABLE);

	// -----------------------------------------------
	// TIM3 32bit timer 1msec time tick에 사용할 timer 초기화
	// -----------------------------------------------
	TimerPeriod = ( (RCC_Clocks.PCLK1_Frequency * 2) / 1000000 ) - 1;

	TIM_TimeBaseStructure.TIM_Prescaler = TimerPeriod;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = (1000 - 1);
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	/* TIM3 counter enable */
	TIM_Cmd(TIM3, ENABLE);
	// -----------------------------------------------
	// TIM5 32bit timer 1usec time tick에 사용할 timer 초기화
	// TIM5는 PCLK1을 2배수한 값이 입력된다
	// -----------------------------------------------
	/* Compute the value to be set in ARR register to generate signal frequency at 1MHz */
	TimerPeriod = ( (RCC_Clocks.PCLK1_Frequency * 2) / 1000000 ) - 1;

	TIM_TimeBaseStructure.TIM_Prescaler = TimerPeriod;	// 1usec;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	// TIM_TimeBaseStructure.TIM_Period = (1000 - 1);	for test
	TIM_TimeBaseStructure.TIM_Period = 0xffffffff;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	/* TIM5 counter enable */
	TIM_Cmd(TIM5, ENABLE);
}



/* ----------------------------------------------------
 *
 *  SDRAM check
 *
 * ---------------------------------------------------- */
int32_t board_RamCheck(void)
{

	int32_t ram_size;
	uint32_t rdata;
	uint32_t rcrc;
	__IO uint32_t *ram_addr;

	// CRC->DR = 0xFFFFFFFF로 만든다
	CRC_ResetDR();

	// --------------------------------------------------
	// Random number 회로를 활성화 한다 -----------------
	// --------------------------------------------------
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);

	/* RNG Peripheral enable */
	RNG_Cmd(ENABLE);

	/* Wait until one RNG number is ready */
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)== RESET) {};
	// --------------------------------------------------

	// sdram bank start address = 0xd000 0000 system dependant
	ram_addr = (__IO uint32_t*)SDRAM_BANK_ADDR;
	ram_size = SDRAM_BANK_SIZE32;	// 32MB를 32bit(4Byte)로 처리한다

#ifdef	DEBUG_MSG
	printf("board: SDRAM Check Start\r\n");
#endif

	while( ram_size-- )
	{
		while( !(RNG->SR & RNG_FLAG_DRDY) );
		rdata = RNG->DR;
		*ram_addr++ = rdata;
		CRC->DR = rdata;

		if ((ram_size % 0x40000) == 0 )
			printf("W");
	}

	rcrc = CRC->DR;

	/* RNG Peripheral disable */
	RNG_Cmd(DISABLE);
	/* Disable RNG clock source */
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, DISABLE);

	printf("\r\nboard: SDRAM CRC1=%08X\r\n", rcrc);


	CRC_ResetDR();

	// sdram bank start address = 0xd000 0000 system dependant
	ram_addr = (__IO uint32_t*)SDRAM_BANK_ADDR;
	ram_size = SDRAM_BANK_SIZE32;	// 32MB를 32bit(4Byte)로 처리한다

	while( ram_size-- )
	{
		CRC->DR = *ram_addr;		// 계산한 후
		*ram_addr++ = 0;			// 메모리 값을 0으로 clear한다

		if ((ram_size % 0x40000) == 0 )
			printf("R");
	}

#ifdef	DEBUG_MSG
	printf("\r\nboard: SDRAM CRC2=%08X\r\n", CRC->DR);
#endif

	if ( rcrc == (uint32_t)CRC->DR )
	{
		ram_size = SDRAM_BANK_SIZE8;
	}
	else ram_size = 0;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, DISABLE);

#ifdef	DEBUG_MSG
	printf("board: SDRAM Check End\r\n");
#endif

	return ram_size;
}

int32_t board_RTCInit(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
//	RTC_AlarmTypeDef RTC_AlarmStructure;

	/* RTC configuration   ---------------------------------- */
	// 외부 RTC가 Enable되어 있는가 확인한다
	if ( (RCC->BDCR & (uint32_t)0x8101) != (uint32_t)0x8101 )
	{	// 보드가 처음으로 초기화가 이루어 졌다

		/* Allow write access to RTC  */
		PWR_BackupAccessCmd(ENABLE);

		/* Enable the LSE OSC */
		RCC_LSEConfig(RCC_LSE_ON);

		/* Wait till LSE is ready */
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
		{
		}

		/* Select the RTC Clock Source */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

		/* Enable the RTC Clock */
		RCC_RTCCLKCmd(ENABLE);

		/* Waits until the RTC Time and Date registers (RTC_TR and RTC_DR) */
		/* are synchronized with RTC APB clock. */
		RTC_WaitForSynchro();

		/* Configure the RTC data register and RTC prescaler */
		RTC_InitStructure.RTC_AsynchPrediv = 0x7f;
		RTC_InitStructure.RTC_SynchPrediv = 0x00ff;
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
		RTC_Init(&RTC_InitStructure);

		/* Set the date: Friday January 1st 2018 */
		RTC_DateStructure.RTC_Year = 0x18;
		RTC_DateStructure.RTC_Month = RTC_Month_January;
		RTC_DateStructure.RTC_Date = 0x1;
		RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Wednesday;
		RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);

		/* Set the time to 05h 20mn 00s AM in UTC */
		RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
		RTC_TimeStructure.RTC_Hours   = 0x05;
		RTC_TimeStructure.RTC_Minutes = 0x20;
		RTC_TimeStructure.RTC_Seconds = 0x00;
		RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);
	}
	else
	{

		// RTC의 Alarm을 읽어서 Power가 꺼진 시간을 알아 낸다
	}


	PWR_BackupAccessCmd(ENABLE);
	// RTC의 Date & Time을 읽어서 보드가 켜진 시간을 기록한다



/*
	// Allow write access to set PC13 as 1Hz
	PWR_BackupAccessCmd(ENABLE);

	// PC13의 AF1을 PPS(1Hz) 출력으로 설정한다
	RTC_OutputTypeConfig(RTC_OutputType_OpenDrain);	// 외부에 pullup을 단다
	RTC_CalibOutputConfig(RTC_CalibOutput_1Hz);
	RTC_CalibOutputCmd(ENABLE);

	PWR_BackupAccessCmd(DISABLE);
*/

	// 사용전력량,
	//

	return 0;

}

/* ----------------------------------------------------
 *
 *  RTC Definitions
 *
 * ---------------------------------------------------- */
#define	NEOPISIEEEE	((uint32_t)(0x00f8769b))
/**
  * @brief  Display the current time.
  * @param  None
  * @retval None
  */

/* ----------------------------------------------------
 *
 *  RNG(Random Number Generator) Definitions
 *
 * ---------------------------------------------------- */
/**
  * @brief  Display the current time.
  * @param  None
  * @retval None
  */

void RNG_Config(void)
{
	/* Enable RNG clock source */
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);

	/* RNG Peripheral enable */
	RNG_Cmd(ENABLE);

	/* Wait until one RNG number is ready */
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)== RESET)
	{
	}

	/* Get a 32bit Random number */
	// random32bit = RNG_GetRandomNumber();

	// 사용하지 않을 때에는 반드시 disable한다
	// clock을 공급하지 않으면 전력을 절약할 수 있다

	/* RNG Peripheral disable */
	RNG_Cmd(DISABLE);

	/* Disable RNG clock source */
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
}

/* ----------------------------------------------------
 *
 *  Hardware Version
 *
 * ---------------------------------------------------- */
/**
  * @brief  Display the current time.
  * @param  None
  * @retval None
  */
int32_t board_HwVer(void)
{
	// key의 초기 값을 정한다
	return ( (int32_t)((GPIOE->IDR>>3) & 0x000F) );
}

/* ----------------------------------------------------
 *
 * Digital Output Definitions
 *
 * ---------------------------------------------------- */
void board_DoInit(void)
{
}

void board_DoSet(uint32_t val)
{
}

void board_TCSSet(void)
{
	// output 출력을 1로한다
	GPIOE->BSRRL = GPIO_Pin_2;

}

void board_TCSReset(void)
{
	// output 출력을 0로한다
	GPIOE->BSRRH = GPIO_Pin_2;

}

void board_TripSet(void)
{
	// output 출력을 1로한다
	GPIOI->BSRRL = GPIO_Pin_4;
}

void board_TripReset(void)
{
	// output 출력을 0로한다
	GPIOI->BSRRH = GPIO_Pin_4;

}

void board_CloseSet(void)
{
	// output 출력을 1로한다
	GPIOI->BSRRL = GPIO_Pin_5;
}

void board_CloseReset(void)
{
	// output 출력을 0로한다
	GPIOI->BSRRH = GPIO_Pin_5;
}

void board_SysFailSet(void)
{
	// SysFail 접점 출력이 B 접점이으로
	// output 출력을 0로한다
	GPIOI->BSRRH = GPIO_Pin_6;
}

void board_SysFailReset(void)
{
	// SysFail 접점 출력이 B 접점이으로
	// output 출력을 1로한다
	GPIOI->BSRRL = GPIO_Pin_6;
}

/* ----------------------------------------------------
 *
 * Analog to Digital Converter Definitions
 *
 * ---------------------------------------------------- */
/* ------------------------------------------------ */
/* 													*/
/* ADC Clock Control functions				*/
/* 													*/
/* ------------------------------------------------ */
void board_DisableAdcTimer(void)
{
	// Interrupt를 멈춘다
	TIM_ITConfig(TIM1, TIM_IT_Update, DISABLE);

	// Timer를 멈춘다
	TIM_Cmd(TIM1, DISABLE);

	// 발생한 Flag를 지운다
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}

void board_EnableAdcTimer(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the TIM1 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	// 최 우선순위
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* TIM1 IT enable */
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
}

void board_SetAdcSampleFrequency(int32_t Hz)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	uint32_t TimerPeriod;

	switch( Hz )
	{
		case F50HZ :
			/* HCLK가 180MHz일 때 TIM1의 50.000Hz 128 sample로 설정한다 */
			TimerPeriod = (28125-1);	// 180000000 / 50 / 128 = 28125
			break;

		case F60HZ :
		default :
			/* HCLK가 180MHz일 때 TIM1의 59.998Hz 128 sample로 설정한다 */
			TimerPeriod = (23438-1);	// 180000000 / 60 / 128 = 23437.5
	}

	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/* TIM1 counter enable */
	TIM_Cmd(TIM1, ENABLE);
}

void board_ResetADChip(void)
{
	// Reset two Chips, AD7606, when PH14 is High
	GPIOH->BSRRL = GPIO_Pin_14;
}

void board_SetADChip(void)
{
	// Set two Chips, AD7606, when PH14 is Low
	GPIOH->BSRRH = GPIO_Pin_14;
}

/* ------------------------------------------------ */
/* 													*/
/* Nand flash memory Control functions				*/
/* 													*/
/* ------------------------------------------------ */
/* Nand flash memory를 write protected 상태로 변경한다 */
void board_SetNandWriteProtect(void)
{
	GPIOH->BSRRH = GPIO_Pin_15;	// Low
}

/* Nand flash memory를 write enabled 상태로 변경한다 */
void board_ResetNandWriteEnable(void)
{
	GPIOH->BSRRL = GPIO_Pin_15;	// Hi
}

void board_TestLed(void)
{
	GPIOF->ODR ^= GPIO_Pin_8;
}


/* ----------------------------------------------------
 *
 *  Definitions
 *
 * ---------------------------------------------------- */
void board_HardwareInit(void)
{
	// Initialize system clocks
	_SysClock_Init();

	// Initialize Genernal Inputs/Outputs
	_GPIO_Init();

	// Initialize debug message Port
	_UART1_Init();

	// Battery Backup Memory를 초기화 한다(Battery Backup RTC Regs and SRAM

}
