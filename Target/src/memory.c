#include "board_fmc.h"
#include "memory.h"

typedef struct node
{
	uint32_t AccessBase;
	uint32_t FreeSize;

} MEM, *PMEM;

static MEM _CCR_M;
static MEM _SDR_M;

void MemoryInit(void)
{
	// CCR RAM을 초기화 한다
	_CCR_M.AccessBase = 0x10000000U;	// 시작 위치
	_CCR_M.FreeSize   = 0x00010000U;	// 64KB 사용가능

	// SDRAM을 초기화 한다
	_SDR_M.AccessBase = SDRAM_BANK_ADDR;	// 시작 위치
	_SDR_M.FreeSize   = SDRAM_BANK_SIZE8;	// 32MB 사용가능
}

void* CCR_malloc(int32_t size)
{	// CPU내부에 있는 64KB의 내부 RAM 영역으로
	// 오로지 CPU만 read/write 가능한다
	// 다른 장치, 즉 DMA, ETH, SPI 등 bus master는 직접  access가 불가능한 영역
	// 스택이나, initialized data 에 적합하다
	uint32_t ret_ptr = 0;

	if ( _CCR_M.FreeSize >= (uint32_t)size )
	{
		ret_ptr = _CCR_M.AccessBase;
		_CCR_M.AccessBase += (uint32_t)size;
		_CCR_M.FreeSize -= (uint32_t)size;
	}

	return( (void*)ret_ptr);
}

void* SDR_malloc(int32_t size)
{
	// SDRAM 영역
	// Promet은 0xD000 0000 ~ 0xD0FF FFFF까지 사용가능한다
	uint32_t ret_ptr = 0;

	if ( _SDR_M.FreeSize >= (uint32_t)size )
	{
		ret_ptr = _SDR_M.AccessBase;
		_SDR_M.AccessBase += (uint32_t)size;
		_SDR_M.FreeSize -= (uint32_t)size;
	}

	return( (void*)ret_ptr);
}


