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
	uint32_t intervaltime;	// config�� ������ original delay
	uint32_t adaptivetime;	// adaptive�ϰ� �����˴� ���� delay
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

// �����쿡�� �Ǿ��� �۾��� ����� ��� ��Ų��
// timer�� ���� ��Ű�� ���� �ð��� ����Ѵ�




#ifdef __cplusplus
}
#endif




#endif
