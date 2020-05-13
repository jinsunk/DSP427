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
	// CCR RAM�� �ʱ�ȭ �Ѵ�
	_CCR_M.AccessBase = 0x10000000U;	// ���� ��ġ
	_CCR_M.FreeSize   = 0x00010000U;	// 64KB ��밡��

	// SDRAM�� �ʱ�ȭ �Ѵ�
	_SDR_M.AccessBase = SDRAM_BANK_ADDR;	// ���� ��ġ
	_SDR_M.FreeSize   = SDRAM_BANK_SIZE8;	// 32MB ��밡��
}

void* CCR_malloc(int32_t size)
{	// CPU���ο� �ִ� 64KB�� ���� RAM ��������
	// ������ CPU�� read/write �����Ѵ�
	// �ٸ� ��ġ, �� DMA, ETH, SPI �� bus master�� ����  access�� �Ұ����� ����
	// �����̳�, initialized data �� �����ϴ�
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
	// SDRAM ����
	// Promet�� 0xD000 0000 ~ 0xD0FF FFFF���� ��밡���Ѵ�
	uint32_t ret_ptr = 0;

	if ( _SDR_M.FreeSize >= (uint32_t)size )
	{
		ret_ptr = _SDR_M.AccessBase;
		_SDR_M.AccessBase += (uint32_t)size;
		_SDR_M.FreeSize -= (uint32_t)size;
	}

	return( (void*)ret_ptr);
}


