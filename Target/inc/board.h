#ifndef _BOARD_H_
#define _BOARD_H_

	#ifndef __STM32F4xx_CONF_H
	#include "stm32f4xx_conf.h"
	#endif

/**
 * defgroup group_common_boards Generic board support
 *
 * The generic board support module includes board-specific definitions
 * and function prototypes, such as the board initialization function.
 *
 *
 */

// 어떤 계전기인지 정의한다
// 정의하는 것에 의해 계전 요소를 물고 들어가게 한다

#define	EXTDEV_AD7606_1		(0x64000000)
#define	EXTDEV_AD7606_2		(0x68000000)

#define	SDRAM_BANK_ADDR		(0xD0000000)
#define	SDRAM_BANK_SIZE8	(0x02000000)		// 32MB
#define	SDRAM_BANK_SIZE32	(SDRAM_BANK_SIZE8/4)


// ADC Start
#define	IO_ADSTART	(GPIOH->BSRRL = GPIO_Pin_13)		/* Bit Set (GPIOI->ODR |= GPIO_Pin_0) */

// ADC Stop
#define	IO_ADSTOP	(GPIOH->BSRRH = GPIO_Pin_13)	/* Bit Reset (GPIOI->ODR &= ~GPIO_Pin_0) */

#define	F60HZ			((int32_t)60)
#define	F50HZ			((int32_t)50)

#define	TICK_UNIT_100US
//#define	TICK_UNIT_1MS

#define	GET_TIME_TICK		TIM2->CNT	// 100usec Tick

#define	GET_TIME_USEC_TICK	TIM5->CNT //  시간체크할때 사용

#ifdef __cplusplus
extern "C" {
#endif

extern int32_t board_HwVer(void);
extern void board_HardwareInit(void);
extern void board_TimerInit(void);
extern void board_InterruptInit(void);


extern int32_t board_RamCheck(void);

extern int32_t board_RTCInit(void);

extern void board_SetNandWriteProtect(void);	// write protected 상태로 변경
extern void board_ResetNandWriteEnable(void);	// write enabled 상태로 변경


/* ----------------------------------- */
/* Nand flash memory Control functions */
/* ----------------------------------- */
/* Nand flash memory를 write protected 상태로 변경한다 */
extern void board_SetNandWriteProtect(void);

/* Nand flash memory를 write enabled 상태로 변경한다 */
extern void board_SetNandWriteEnable(void);


/* ----------------------------------- */
/* RTC Control functions */
/* ----------------------------------- */
//extern void board_SysRtcInit(void);
//extern void board_SysRtcRun(void);
//extern void board_SysRtcStop(void);


/* ----------------------------------- */
/* Console functions */
/* ----------------------------------- */
#define	CONPORT			USART1
#define	CONPORT_IRQ		USART1_IRQn
#define	CONPORT_HANDLER	USART1_IRQHandler

void _UART1_Init(void);
/* ------------------------------------- */
/* Analog to Digital Converter functions */
/* ------------------------------------- */
extern void board_ResetADChip(void);
extern void board_SetADChip(void);
extern void board_DisableAdcTimer(void);
extern void board_EnableAdcTimer(void);
extern void board_SetAdcSampleFrequency(int32_t Hz);

/* ----------------------------------- */
/*  functions */
/* ----------------------------------- */
extern void board_TestLed(void);

/* ----------------------------------- */
/* UART functions */
/* ----------------------------------- */
extern void board_ExtUartInit(void);
extern void board_ExtUartResetHW(void);
extern void board_ExtUartSetHW(void);

/* ---------------------------------------- */
/* ADC Clock Control functions				*/
/* ---------------------------------------- */
// adc에서 사용할 clock을 만들어 낸다 2mhz
extern void board_ResetADC_Clock(void);
extern void board_SetADC_Clock(void);


/* ----------------------------------- */
/* DO Control functions */
/* ----------------------------------- */
extern void board_DoInit(void);
extern void board_DoSet(uint32_t val);

extern void board_TripSet(void);
extern void board_TripReset(void);

extern void board_CloseSet(void);
extern void board_CloseReset(void);

extern void board_SysFailSet(void);
extern void board_SysFailReset(void);

void board_TCSSet(void);
void board_TCSReset(void);

#ifdef __cplusplus
}
#endif

/**
 * \}
 */



#endif  // _BOARD_H_
