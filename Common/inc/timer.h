#ifndef	_TIMER_H_
#define	_TIMER_H_

	#ifndef _LIST_H_
	#include "list.h"
	#endif

	#ifndef	_MSGTYPEDEF_H_
	#include "msgtypedef.h"
	#endif

typedef struct {
	uint32_t expiretimetick;
	uint32_t interval;

	MESSAGE message;
} MSGTIMER, *PMSGTIMER;

#ifdef __cplusplus
extern "C" {
#endif

extern void InitTimerMessage(void);
extern int32_t GetTimerMessage(PMESSAGE);
extern PNODE SendTimerMessage(callbackmsg_t hwnd, int32_t msg, int32_t params, uint32_t interval);
// extern void RemoveTimerMessage(void *owner, PNODE pNode);
extern void RemoveTimerMessage(PNODE *pNode);


#ifdef __cplusplus
}
#endif

#endif	// _TIMER_H_
