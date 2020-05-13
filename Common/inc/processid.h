#ifndef	_PROCESSID_H_
#define	_PROCESSID_H_

	#ifndef _STDINT
	#include <stdint.h>
	#endif

#define	MAX_PID		10

// process로 정의되는 모든 Identification에 대하여 정의한다
#define	PID_UNKNOWN		((int32_t)0)

#define	PID_ROOT		((int32_t)1)
#define	PID_LCDTXT		((int32_t)2)
#define	PID_CONSOLE		((int32_t)3)
#define PID_RLYDBG		((int32_t)4)
#define PID_SNTP		((int32_t)5)
#define PID_MOBUSTCP	((int32_t)6)
#define PID_SPI			((int32_t)7)








#ifdef __cplusplus
extern "C" {
#endif

extern void pid_Init(void);
extern void *pid_GetProcess(int32_t idx);
extern int32_t pid_SetProcess(int32_t idx, void *);

#ifdef __cplusplus
}
#endif


#endif	// _PROCESSID_H_
