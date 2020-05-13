#include "map.h"


// METi2500S2의 Map
// RTC backup memory에 저장할 내용
// 3상 전력량 정수부, import, export		32bit reg x 2
// 3상 전력량 소수부, import, export		32bit reg x 2



uint32_t  temp_buff32[TEMP_BUFF_MAX/4];
uint16_t *temp_buff16 = (uint16_t*)temp_buff32;
uint8_t  *temp_buff8 =   (uint8_t*)temp_buff32;


static MAP_J6300P map_J6300p;


void map_SetWarning(uint32_t u32_flag)
{
	map_J6300p.warn_flag |= u32_flag;
}

void map_ResetWarning(uint32_t u32_flag)
{
	map_J6300p.warn_flag &= ~u32_flag;
}

void map_SetError(uint32_t u32_flag)
{
	map_J6300p.error_flag |= u32_flag;
}

void map_ResetError(uint32_t u32_flag)
{
	map_J6300p.error_flag &= ~u32_flag;
}

void map_SetState(uint32_t u32_flag)
{
	map_J6300p.state_flag |= u32_flag;
}

void map_ResetState(uint32_t u32_flag)
{
	map_J6300p.state_flag &= ~u32_flag;
}
