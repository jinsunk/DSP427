#include "board_fmc.h"

/**
  * @brief  FMC SDRAM Mode definition register defines
  */
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

void SDRAM_InitSequence(void);

static void delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0;
  for(index = (100000 * nCount); index != 0; index--)
  {
  }
}


void FMC_BANK1_Region1_Init(void)
{
	// BANK1 Region1 Start Address	0x6000 0000
	// No Device
}

void FMC_BANK1_Region2_Init(void) //ADC
{
	// BANK1 Region1 Start Address	0x6400 0000
	FMC_NORSRAMInitTypeDef  FMC_NORSRAMInitStructure;
	FMC_NORSRAMTimingInitTypeDef  p;

	p.FMC_AddressSetupTime = 0x2;
	p.FMC_AddressHoldTime = 2;
	p.FMC_DataSetupTime = 0x6;
	p.FMC_BusTurnAroundDuration = 5;
	p.FMC_CLKDivision = 1;				// don't care in mode A
	p.FMC_DataLatency = 0;				// don't care in mode A
	p.FMC_AccessMode = FMC_AccessMode_A;

	FMC_NORSRAMInitStructure.FMC_Bank = FMC_Bank1_NORSRAM2;
	FMC_NORSRAMInitStructure.FMC_DataAddressMux = FMC_DataAddressMux_Disable;
	FMC_NORSRAMInitStructure.FMC_MemoryType = FMC_MemoryType_SRAM;
	FMC_NORSRAMInitStructure.FMC_MemoryDataWidth = FMC_NORSRAM_MemoryDataWidth_16b;
	FMC_NORSRAMInitStructure.FMC_BurstAccessMode = FMC_BurstAccessMode_Disable;
	FMC_NORSRAMInitStructure.FMC_AsynchronousWait = FMC_AsynchronousWait_Disable;

	// Errta 2.8.2
	FMC_NORSRAMInitStructure.FMC_WaitSignalPolarity = FMC_WaitSignalPolarity_High;

	FMC_NORSRAMInitStructure.FMC_WrapMode = FMC_WrapMode_Disable;
	FMC_NORSRAMInitStructure.FMC_WaitSignalActive = FMC_WaitSignalActive_BeforeWaitState;
	FMC_NORSRAMInitStructure.FMC_WriteOperation = FMC_WriteOperation_Enable;
	FMC_NORSRAMInitStructure.FMC_WaitSignal = FMC_WaitSignal_Disable;
	FMC_NORSRAMInitStructure.FMC_ExtendedMode = FMC_ExtendedMode_Enable;
	FMC_NORSRAMInitStructure.FMC_WriteBurst = FMC_WriteBurst_Disable;
	FMC_NORSRAMInitStructure.FMC_ReadWriteTimingStruct = &p;
	FMC_NORSRAMInitStructure.FMC_WriteTimingStruct = &p;

	FMC_NORSRAMInit(&FMC_NORSRAMInitStructure);

	/*!< Enable FSMC Bank1_SRAM3 Bank */
	FMC_NORSRAMCmd(FMC_Bank1_NORSRAM2, ENABLE);
}


void FMC_BANK1_Region3_Init(void)  //ADC 2
{
	// BANK1 Region3 Start Address	0x6800 0000
	/* DPRAM 삭제.(사용안함. ADC로 변경됨. DSP 보드 0.3b2 부터 적용
	// Dual Port RAM IDT70V261, Control via Left Side
	// 16K x 16bit( 0x6800 0000 ~ 0x6800 7FFF)
	// to Set Interrupt Flag of Right Port, Write a datum to 0x6800 7FFF
	// to avoid setting interrupt flag bit, memory test should be within
	// from 0x6800 0000 to 0x6800 7ffb
*/
	FMC_NORSRAMInitTypeDef  FMC_NORSRAMInitStructure;
	FMC_NORSRAMTimingInitTypeDef  p;

	p.FMC_AddressSetupTime = 0x2;
	p.FMC_AddressHoldTime = 2;
	p.FMC_DataSetupTime = 0x6;
	p.FMC_BusTurnAroundDuration = 5;
	p.FMC_CLKDivision = 1;				// don't care in mode A
	p.FMC_DataLatency = 0;				// don't care in mode A
	p.FMC_AccessMode = FMC_AccessMode_A;

	FMC_NORSRAMInitStructure.FMC_Bank = FMC_Bank1_NORSRAM3;
	FMC_NORSRAMInitStructure.FMC_DataAddressMux = FMC_DataAddressMux_Disable;
	FMC_NORSRAMInitStructure.FMC_MemoryType = FMC_MemoryType_SRAM;
	FMC_NORSRAMInitStructure.FMC_MemoryDataWidth = FMC_NORSRAM_MemoryDataWidth_16b;
	FMC_NORSRAMInitStructure.FMC_BurstAccessMode = FMC_BurstAccessMode_Disable;
	FMC_NORSRAMInitStructure.FMC_AsynchronousWait = FMC_AsynchronousWait_Disable;

	// Errta 2.8.2
	FMC_NORSRAMInitStructure.FMC_WaitSignalPolarity = FMC_WaitSignalPolarity_High;

	FMC_NORSRAMInitStructure.FMC_WrapMode = FMC_WrapMode_Disable;
	FMC_NORSRAMInitStructure.FMC_WaitSignalActive = FMC_WaitSignalActive_BeforeWaitState;
	FMC_NORSRAMInitStructure.FMC_WriteOperation = FMC_WriteOperation_Enable;
	FMC_NORSRAMInitStructure.FMC_WaitSignal = FMC_WaitSignal_Disable;
	FMC_NORSRAMInitStructure.FMC_ExtendedMode = FMC_ExtendedMode_Enable;
	FMC_NORSRAMInitStructure.FMC_WriteBurst = FMC_WriteBurst_Disable;
	FMC_NORSRAMInitStructure.FMC_ReadWriteTimingStruct = &p;
	FMC_NORSRAMInitStructure.FMC_WriteTimingStruct = &p;

	FMC_NORSRAMInit(&FMC_NORSRAMInitStructure);

	/*!< Enable FSMC Bank1_SRAM3 Bank */
	FMC_NORSRAMCmd(FMC_Bank1_NORSRAM3, ENABLE);
}

void FMC_BANK1_Region4_Init(void)
{
	// BANK1 Region4 Start Address	0x6C00 0000
	// No Device
}

void FMC_BANK1_Init(void)
{
	// BANK1 Start Address	0x6000 0000
	// BANK1 has 4 regions.
	FMC_BANK1_Region1_Init();
	FMC_BANK1_Region2_Init();
	FMC_BANK1_Region3_Init();
	FMC_BANK1_Region4_Init();
}

void FMC_BANK2_Init(void)
{
	// Nand Flash((Not Used))
}

void FMC_BANK3_Init(void)
{

	// Nand Flash((Not Used))
}

void FMC_BANK4_Init(void)
{
	// BANK4 Start Address	0x9000 0000
	// PC Card(Not Used)
}

void FMC_BANK5_Init(void)
{
	// BANK5 Start Address	0xA000 0000
	// SDRAM(Not Used)
}




/* Notice : Errta 2.8.1 - Dummy read cycles inserted when reading synchronous memores */
/* sync memory 즉 sdram과 같은 동기클락을 갖는 메모리를 burst mode로 read를 수행하면
그 끝나는 시점에 2개의 쓸대 없는 read가 발생하지만(시간은 소요되지만) 데이터에는 지장 없음 */
void FMC_BANK6_Init(void)
{
	// BANK6 Start Address	0xD000 0000, Size = 0x02000000
	// SDRAM referenced by K4S561632
	FMC_SDRAMInitTypeDef  FMC_SDRAMInitStructure;
	FMC_SDRAMTimingInitTypeDef  FMC_SDRAMTimingInitStructure;

	/* FMC Configuration ---------------------------------------------------------*/
	/* FMC SDRAM Bank configuration */
	/* Timing configuration for 84 Mhz of SD clock frequency (168Mhz/2) */
	/* TMRD: 2 Clock cycles */
	FMC_SDRAMTimingInitStructure.FMC_LoadToActiveDelay    = 2;
	/* TXSR: min=70ns (7x11.11ns) */
	FMC_SDRAMTimingInitStructure.FMC_ExitSelfRefreshDelay = 7;
	/* TRAS: min=42ns (4x11.11ns) max=120k (ns) */
	FMC_SDRAMTimingInitStructure.FMC_SelfRefreshTime      = 4;
	/* TRC:  min=70 (7x11.11ns) */
	FMC_SDRAMTimingInitStructure.FMC_RowCycleDelay        = 7;
	/* TWR:  min=1+ 7ns (1+1x11.11ns) */
	FMC_SDRAMTimingInitStructure.FMC_WriteRecoveryTime    = 2;
	/* TRP:  20ns => 2x11.11ns */
	FMC_SDRAMTimingInitStructure.FMC_RPDelay              = 2;
	/* TRCD: 20ns => 2x11.11ns */
	FMC_SDRAMTimingInitStructure.FMC_RCDDelay             = 2;

	/* FMC SDRAM control configuration */
	FMC_SDRAMInitStructure.FMC_Bank = FMC_Bank2_SDRAM;
	/* Column addressing: [8:0] - modified by Sooan */
	FMC_SDRAMInitStructure.FMC_ColumnBitsNumber = FMC_ColumnBits_Number_9b;
	/* Row addressing: [12:0] - modified by Sooan */
	FMC_SDRAMInitStructure.FMC_RowBitsNumber = FMC_RowBits_Number_13b;

	FMC_SDRAMInitStructure.FMC_SDMemoryDataWidth = FMC_SDMemory_Width_16b;
	FMC_SDRAMInitStructure.FMC_InternalBankNumber = FMC_InternalBank_Number_4;
	FMC_SDRAMInitStructure.FMC_CASLatency = FMC_CAS_Latency_3;
	FMC_SDRAMInitStructure.FMC_WriteProtection = FMC_Write_Protection_Disable;
	FMC_SDRAMInitStructure.FMC_SDClockPeriod = FMC_SDClock_Period_2;

	// Errta 2.8.5 : Interruption of CPU read burst access to an end of SDRAM row
	// to solve this enalbes the read FIFO FMC_SDCR1->RBURST = 1
	FMC_SDRAMInitStructure.FMC_ReadBurst = FMC_Read_Burst_Enable;
//	FMC_SDRAMInitStructure.FMC_ReadBurst = FMC_Read_Burst_Disable;

	FMC_SDRAMInitStructure.FMC_ReadPipeDelay = FMC_ReadPipe_Delay_1;
	FMC_SDRAMInitStructure.FMC_SDRAMTimingStruct = &FMC_SDRAMTimingInitStructure;

	/* FMC SDRAM bank initialization */
	FMC_SDRAMInit(&FMC_SDRAMInitStructure);

	/* FMC SDRAM device initialization sequence */
	SDRAM_InitSequence();
}

/**
  * @brief  Executes the SDRAM memory initialization sequence.
  * @param  None.
  * @retval None.
  */
void SDRAM_InitSequence(void)
{
	FMC_SDRAMCommandTypeDef FMC_SDRAMCommandStructure;
	uint32_t tmpr = 0;

/* Step 3 --------------------------------------------------------------------*/
	/* Configure a clock configuration enable command */
	FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_CLK_Enabled;
	FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank2;
	FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
	FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
	/* Wait until the SDRAM controller is ready */
	while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
	{
	}
	/* Send the command */
	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

/* Step 4 --------------------------------------------------------------------*/
	/* Insert 100 ms delay */
	delay(10);

/* Step 5 --------------------------------------------------------------------*/
	/* Configure a PALL (precharge all) command */
	FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_PALL;
	FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank2;
	FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
	FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
	/* Wait until the SDRAM controller is ready */
	while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
	{
	}
	/* Send the command */
	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

/* Step 6 --------------------------------------------------------------------*/
	/* Configure a Auto-Refresh command */
	FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_AutoRefresh;
	FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank2;
	FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 4;
	FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
	/* Wait until the SDRAM controller is ready */
	while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
	{
	}
	/* Send the  first command */
	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

	/* Wait until the SDRAM controller is ready */
	while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
	{
	}
	/* Send the second command */
	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

/* Step 7 --------------------------------------------------------------------*/
	/* Program the external memory mode register */
	tmpr = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2          |
				   SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
				   SDRAM_MODEREG_CAS_LATENCY_3           |
				   SDRAM_MODEREG_OPERATING_MODE_STANDARD |
				   SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

	/* Configure a load Mode register command */
	FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_LoadMode;
	FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank2;
	FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
	FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = tmpr;
	/* Wait until the SDRAM controller is ready */
	while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
	{
	}
	/* Send the command */
	FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);

/* Step 8 --------------------------------------------------------------------*/

	/* Set the refresh rate counter */
	/* (15.62 us x Freq) - 20 */
	/* Set the device refresh counter */
	FMC_SetRefreshCount(1386);
	/* Wait until the SDRAM controller is ready */
	while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
	{
	}
}

/**
  * @brief  Writes a Entire-word buffer to the SDRAM memory.
  * @param  pBuffer: pointer to buffer.
  * @param  uwWriteAddress: SDRAM memory internal address from which the data will be
  *         written.
  * @param  uwBufferSize: number of words to write.
  * @retval None.
  */
void SDRAM_WriteBuffer(uint32_t* pBuffer, uint32_t uwWriteAddress, uint32_t uwBufferSize)
{
	__IO uint32_t write_pointer = (uint32_t)uwWriteAddress;

	/* Disable write protection */
	FMC_SDRAMWriteProtectionConfig(FMC_Bank2_SDRAM, DISABLE);

	/* Wait until the SDRAM controller is ready */
	while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
	{
	}

	/* While there is data to write */
	for (; uwBufferSize != 0; uwBufferSize--)
	{
		/* Transfer data to the memory */
		*(uint32_t *) (SDRAM_BANK_ADDR + write_pointer) = *pBuffer++;

		/* Increment the address*/
		write_pointer += 4;
	}
}

/**
  * @brief  Reads data buffer from the SDRAM memory.
  * @param  pBuffer: pointer to buffer.
  * @param  ReadAddress: SDRAM memory internal address from which the data will be
  *         read.
  * @param  uwBufferSize: number of words to write.
  * @retval None.
  */
void SDRAM_ReadBuffer(uint32_t* pBuffer, uint32_t uwReadAddress, uint32_t uwBufferSize)
{
	__IO uint32_t write_pointer = (uint32_t)uwReadAddress;


	/* Wait until the SDRAM controller is ready */
	while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
	{
	}

	/* Read data */
	for(; uwBufferSize != 0x00; uwBufferSize--)
	{
		*pBuffer++ = *(__IO uint32_t *)(SDRAM_BANK_ADDR + write_pointer );

		/* Increment the address*/
		write_pointer += 4;
	}
}

void FMC_GPIO_Init(void)
{
	// 여기서 하지않고 board.c에서 일괄 처리한다
}

void FMC_BANK_Init(void)
{
	/* Enable FMC clock */
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);

	FMC_BANK1_Init();
	//FMC_BANK2_Init(); /nand flash 별도 초기화
	FMC_BANK3_Init();
	FMC_BANK4_Init();
	FMC_BANK5_Init();
	FMC_BANK6_Init();
}
