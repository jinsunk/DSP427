
#ifndef Define_H
#define Define_H

#include <stdio.h>

#include "arm_math.h"

/*
#ifndef	_TYPEDEF_H_
#include "typedef.h"
#endif
*/


// #define Type_T


#ifndef _BOARD_H_
#include "board.h"
#endif


//#define	MAX_ANALOG_CHANNEL			14



#define         SAMPLE_RATE		128
#define         SYSTEM_FREQ	F60HZ
#define         SEC_MEAN_FACTOR      1/SYSTEM_FREQ  //1초 평균을 구하기 위해
#define         HOUR_MEAN_FACTOR       0.00027777778  //시간:초 비율

#define	NodeID_slot1		1
#define	NodeID_slot2		3
#define	NodeID_slot3		4

#define board_Main	0x20
#define board_Sub		0x21


#define SAMPLING_CYCLE	128
#define MAX_SAMPLIE_BUFF	SAMPLING_CYCLE*2
#define SAMPLING_NUM	32
#define SAMPLING_RECIPROCAL	(float)1/SAMPLING_NUM
#define Sampling_DFT	0x01
#define Sampling_TRUERMS	0x02
#define Sampling_Algorism	0x04
#define Sampling_Calculation 	0x08
#define Freq_Calculation 	0x10




#define ID_OPER_RECORD	0x01
#define ID_FAULT_RECORD	0x02

/* OPER_RECORD(SER) 상위 8bit 구분. */
#define ID_POWER					0x01
#define ID_ERROR						0x02
#define ID_DIO_STATE				0x04
#define ID_PICKUP_STATE			0x08
#define ID_FLT_STATE				0x10
#define ID_CB_STATE				0x20
#define ID_SETTING_CHANGE		0x40
#define ID_ETC							0x80


/* OPER_RECORD(SER) 하위 구분. */

/* ID_POWER  */
#define ID_POWER_ON			0x10//watchdog,  on
#define ID_POWER_WDG		0x20//watchdog,  on
/* ID_ERROR  */
#define ID_ERR_MEMORY				0x01
#define ID_ERR_SET						0x02	  	//SETTING 범위/TAP 이상.
#define ID_ERR_PWR						0x04
#define ID_ERR_ADC						0x08
#define ID_ERR_RTC 						0x10
#define	ID_ERR_DIO_COMM			0x20
#define ID_ERR_NETWORK_COMM	0x40
/* ID_CB_STATE  */
#define	ID_CB_ON		0x01
#define ID_CB_OFF	0x02
/* ID_SETTING_CHANGE  */
#define	ID_SET_RELAY				0x01
#define ID_SET_SYSCONFIG		0x02
#define ID_SET_CALIBRATION	0x04
#define ID_SET_COMM 				0x08
/* ID_ETC  */
#define ID_RE_LO  				0x01 //remote local
#define ID_FLT_RESET			0x02
#define ID_DLE_OPER			0x04
#define ID_DLE_FAULT			0x08
#define ID_DLE_WAVE			0x10


#define ID_Factory			0x1000	//FACTORY 모드 실행
#define ID_TEST				0x2000	//TEST

#define ID_SET_UPDATE_RELAY		0x10000
#define ID_SET_UPDATE_COMM		0x20000
#define ID_SET_UPDATE_SYS		0x40000



#define flash_ready	0
#define flash_buff		1
#define flash_save	2

#define OP_MAXNUM	1024
#define FLT_MAXNUM	1024

#define oper_buff_size	5
#define flt_buff_size	38

#define WIRE_3P3W		3
#define WIRE_3P4W		4

#define	LOCAL_			1
#define	REMOTE_		2
#define	SERIAL_			3


#define     I_PHSA            0
#define     I_PHSB            1
#define     I_PHSC            2
#define     I_PHSN            3
#define     I_PHSO            4

#define     V_PHSA            5
#define     V_PHSB            6
#define     V_PHSC            7
#define     V_PHSN            8
#define     V_PHSO            9
#define     V_PHSF           V_PHSA

/*T버전 2차전류*/
#define     I_PHSR           	10
#define     I_PHSS            	11
#define     I_PHST            	12
#define     I_PHSN_S            13

#define     I_PHSO_S			14   //온도센서  (adc 온도 보상시 사용 )


#define     I_PHSAB           0
#define     I_PHSBC           1
#define     I_PHSCA           2
#define     V_PHSAB           3
#define     V_PHSBC           4
#define     V_PHSCA           5



/*
#define Oper_PICK_ON	0x1001
#define Oper_PICK_OFF 	0x1002
#define Oper_FLT_ON		0x1003
#define Oper_FLT_OFF 	0x1004
#define Oper_SET	0x1005
#define Oper_DO	0x1006
*/
#define FLT_  0x20008
/*---------------------------------------------------------------
MOTOR Status
 MT_Stop : 3상 전류가 모두 cutoff전류 이하
 MT_Start : Motor 기동 중.  Stop 상태에서 한 개의 상 이상의 전류가 일정 수준 이상일 때.
 MT_Nomal :  Motor 정상상태
 MT_Overload : 과부하.
-------------------------------------------------------------------*/

/*
#define Event_F 0x01
#define Event_W 0x02
*/

#define MT_Stop			0
#define MT_Start			1
#define MT_Nomal 		2
#define MT_Overload	3

#define DEMAND_TIME	15

#define P_reward	0
#define P_forward	1

#define RELAY_TRIP		0x01
#define RELAY_CLOSE	0x02
#define	RELAY_ON		0x01
#define RELAY_OFF		0x00

/*
//전류 전압 값 배열 순서가 섞이면 사용

typedef struct AnalogInput{

	float32_t Cur[5];
	float32_t Cur_Ang[5];
	float32_t Vol[5];
	float32_t Vol_Ang[5];
}Analog_value;

*/

typedef struct {
 	int32_t mag;  //*1000
    int32_t ang;  //*100
} ComplexInfo_int;



typedef struct {
	float32_t curr[5];
	float32_t volt[5];
#ifdef Type_T
	float32_t curr_2[5];
#endif
	float32_t fMeanP;
	float32_t fMeanQ;
	float32_t fMeanS;
	float32_t fMeanPF;
	float32_t fMeanHz;  //평균 주파수
//	float32_t fMeanHz_2;
}DEMAND_INFO;



/*
#define Max_slot1_main	16
#define Max_slot1_sub	8
#define Max_slot2_main	16
#define Max_slot2_sub	16
#define Max_slot3_main	16
*/

#define Max_slot_num	16

typedef struct DIO_INFO
{
	uint8_t DIO_type;
	uint8_t DIO_num;
	uint8_t DIO_State[Max_slot_num];

}DIO_INFO;

typedef struct SLOT
{

	DIO_INFO Slot1_main;
	DIO_INFO Slot1_sub;
	DIO_INFO Slot2_main;
	DIO_INFO Slot2_sub;
	DIO_INFO Slot3_main;

}SLOT_INFO  ;

extern SLOT_INFO DIO_SLOT;
#endif