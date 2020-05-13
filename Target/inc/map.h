#ifndef _MAP_H_
#define _MAP_H_


#ifndef	_STDINT
#include <stdint.h>
#endif	// _STDINT


#define		ERR_UNHARDWARE	(0x00000002)	// Hardware 정보가 이상하다(Ver & MAC)


#define		ERR_CLCD			(0x00000101)	// LCD에 이상


/* Ethernet 관련 */
#define		ERR_HWPHY			(0x00000100)	// PHY chip에 문제가 있을 때
#define		ERR_ETH0_PORT1		(0x00000001)	// Eth0 Port1 link fail
#define		ERR_ETH0_PORT2		(0x00000002)	// Eth0 Port2 link fail

#define		ERR_HWETH0			(0x00000200)	// ETH0 chip에 문제가 있을 때
#define		ERR_SOCK0			(0x00000001)	//
#define		ERR_SOCK1			(0x00000002)	//
#define		ERR_SOCK2			(0x00000004)	//
#define		ERR_SOCK3			(0x00000008)	//
#define		ERR_SOCK4			(0x00000010)	//
#define		ERR_SOCK5			(0x00000020)	//
#define		ERR_SOCK6			(0x00000040)	//
#define		ERR_SOCK7			(0x00000080)	//

#define		ERR_SWETH0			(0x00000400)	// ETH0 설정에 문제가 있을 때
#define		ERR_IPCONFLCT		(0x00000001)	// 중복된 IP address 검출
#define		ERR_UNDESTINE		(0x00000002)	// 해당 deatine을 찾을수 없다
#define		ERR_MAC				(0x00000004)	// MAC address를 찾을 수 없다
/* */
#define		ERR_TIMESYNC		(0x00000004)


typedef struct {
	uint32_t	test;

	uint8_t		mac[8];

	uint8_t		ip[4];
	uint8_t		subnet[4];
	uint8_t		gateway[4];

	uint32_t	warn_flag;
	uint32_t	error_flag;
	uint32_t	state_flag;


//	Acuvim II	Holding Reg
	uint16_t	SetFreqency;	// 0x0ffd	0	0:50Hz, 1:60Hz
	uint16_t	SetProrocol;	// 0x0ffe	0	0:Mdobus, 1:DNP3.0
	uint16_t	SetPerity;		// 0x0fff	3	0:Even, 1:Odd, 2: None
	uint16_t	SetPassword;	// 0x1000			0 ~ 9999	0
	uint16_t	SetDeviceID;	// 0x1001			0~65534	1	1~248(MODBUS), 0~65534(DNP3.0)
	uint16_t	SetBaudRate;	// 0x1002	19200	1200~38400
	uint16_t	SetVwireType;	// 0x1003	0		0	0:3LN, 1:1LN, 2:2LL, 3:3LL, 4:1LL
	uint16_t	SetIwireType;	// 0x1004	0		0	0:3CT, 1:1CT, 2:2CT
	uint16_t	SetPT1RateH;	// 0x1005
	uint16_t	SetPT1RateL;	// 0x1006
	uint16_t	SetPT2Rate;		// 0x1007
} MAP_J6300P, PMAP_J6300P;


#define	TEMP_BUFF_MAX	(4096)

extern uint32_t	temp_buff32[];
extern uint16_t	*temp_buff16;
extern uint8_t	*temp_buff8;

#ifdef __cplusplus
extern "C" {
#endif

extern void map_SetWarning(uint32_t u32_flag);
extern void map_ResetWarning(uint32_t u32_flag);

extern void map_SetError(uint32_t u32_flag);
extern void map_ResetError(uint32_t u32_flag);

extern void map_SetState(uint32_t u32_flag);
extern void map_ResetState(uint32_t u32_flag);


#ifdef __cplusplus
}
#endif

#endif

