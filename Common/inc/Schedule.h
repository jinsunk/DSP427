#ifndef	_SCHEDULE_H_
#define	_SCHEDULE_H_

	#ifndef _STDINT
	#include <stdint.h>
	#endif

	#ifndef	_LIST_H_
	#include "list.h"
	#endif

#define	MAX_SFRAME	8
typedef struct {
	uint32_t expiretime;
	uint32_t intervaltime;	// config에 설정된 original delay
	uint32_t adaptivetime;	// adaptive하게 가변죄는 가변 delay
	uint32_t frame[MAX_SFRAME];

}  STAG, *PSTAG;	// schdeuler Tag

#ifdef __cplusplus
extern "C" {
#endif

extern int32_t InitSchedule(PLIST pScheduleList);
extern PNODE AddNewSchedule(PLIST pS, PSTAG pT);

extern int32_t FlushSchedule(PLIST pS);
extern int32_t DistroySchedule(PLIST pS);

extern int32_t UpdatetSchedule(PLIST pS);
extern int32_t UpdatetScheduleDelay(PLIST pS, int32_t intv);

extern int32_t ProbeSchedule(PLIST pS);
extern int32_t ReadSchedule(PLIST pS, PSTAG pT);

// 스케쥴에서 맨앞의 작업을 때어내어 대기 시킨다
// timer를 가동 시키기 위한 시간을 취득한다




#ifdef __cplusplus
}
#endif




#endif
