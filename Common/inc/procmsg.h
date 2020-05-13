#ifndef	_PROCMSH_H_
#define	_PROCMSH_H_

	#ifndef _LIST_H_
	#include "list.h"
	#endif

	#ifndef	_MSGTYPEDEF_H_
	#include "msgtypedef.h"
	#endif


#ifdef __cplusplus
extern "C" {
#endif

extern void InitMessage(void);
extern int32_t SendMessage(callbackmsg_t hprc, int32_t msg, int32_t param, int32_t priority);
extern int32_t GetMessage(PMESSAGE);

#ifdef __cplusplus
}
#endif


#endif	// _PROCMSH_H_
