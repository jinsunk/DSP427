#ifndef _BOARD_FMC_H_
#define _BOARD_FMC_H_

#ifndef __STM32F4xx_CONF_H
#include <stm32f4xx_conf.h>
#endif
#ifndef _ADC_H_
#include "adc.h"
#endif

/**
  * @brief  FMC SDRAM Bank address
	BANK6
  */
#define SDRAM_BANK_ADDR     ((uint32_t)0xD0000000U)
#define SDRAM_BANK_SIZE8    ((uint32_t)0x02000000U)	// K4S561632(32MB)
#define SDRAM_BANK_SIZE16   ((uint32_t)SDRAM_BANK_SIZE8/2)
#define SDRAM_BANK_SIZE32   ((uint32_t)SDRAM_BANK_SIZE16/2)


#define SDRAM_RINGBUFF_SIZE (RING_SIZE*MAX_ACH*2)
#define SDRAM_RINGBUFF_ADDR	SDRAM_FLT_ADDR+SDRAM_FLT_SIZE+0x100//SDRAM_BANK_ADDR

#define SDRAM_OPER_ADDR	SDRAM_BANK_ADDR//SDRAM_RINGBUFF_ADDR+SDRAM_RINGBUFF_SIZE+0x100		//SDRAM_BANK_ADDR//(SDRAM_RINGBUFF_ADDR+SDRAM_RINGBUFF_SIZE)+100
#define SDRAM_OPER_SIZE	(OP_MAXNUM*oper_buff_size)*2+4

#define SDRAM_FLT_ADDR	SDRAM_OPER_ADDR+SDRAM_OPER_SIZE+0x100		//SDRAM_BANK_ADDR//(SDRAM_RINGBUFF_ADDR+SDRAM_RINGBUFF_SIZE)+100
#define SDRAM_FLT_SIZE	FLT_MAXNUM*flt_buff_size*2+4


/*
FLT WAVE 파형 1개당 길이(30s 기준)
//header//
FLT_INFO  64bit(8Byte)
FLT_TIME  64bit(8Byte)
FLT_TRIGGER_NUM 8bit(1Byte)
Start_Time 64bit(8Byte)
Frequency 16bit(2Byte)
calibration 32bit(4Byte) *12ch,
=============================
75byte

//data//
Analog signal 16bit(2Byte)*12ch
Fault info 64bit(8Byte)
Pickup info 64bit(8Byte)
DI	32bit(4Byte)
DO	32bit(4Byte)
Frequency  16bit (2Byte)
=============================
50byte *128sample*60hz*30s = 11,520,000B 11,250KB  10.986328MB

11520075B=11,250.0732KB=10.9864MB

*/


#ifdef __cplusplus
extern "C" {
#endif

extern void FMC_GPIO_Init(void);
extern void FMC_BANK_Init(void);
extern void SDRAM_WriteBuffer(uint32_t* pBuffer, uint32_t uwWriteAddress, uint32_t uwBufferSize);
extern void SDRAM_ReadBuffer(uint32_t* pBuffer, uint32_t uwReadAddress, uint32_t uwBufferSize);

#ifdef __cplusplus
}
#endif

/**
 * \}
 */



#endif  // _BOARD_FMC_H_
