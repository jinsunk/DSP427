#ifndef	_MSGTYPEDEF_H_
#define	_MSGTYPEDEF_H_

#ifndef _STDINT
#include <stdint.h>
#endif


typedef void (*callbackmsg_t)(int32_t, int32_t);
typedef int32_t  (*callbackcmp_t)(void);
typedef int32_t  (*callbackf_t)(uint8_t*, int32_t);
typedef int32_t  (*callbackentf_t)(int32_t);
typedef int32_t  (*callbackcfgf_t)(int32_t, void *);

typedef int32_t  (*callbackf_tranceive)(int32_t, void *, int32_t);

// #define	__MSG_TIME_STAMP__
// my message type
typedef struct {
	callbackmsg_t	callfunc;	// 메세지가 수행될 function
	int32_t		message;	// message의 종류
	int32_t		params;		// 전달 인자, value 또는 pointer일 수 있다.

#ifdef	__MSG_TIME_STAMP__
	time_t	seconds;			// 생성시각
	time_t	fraction;
#endif
	//int process id ??	// 발생한 message 순서를 기록하는 ID
} MESSAGE, *PMESSAGE;

typedef struct {
	int32_t PID;
	int32_t PUN;
	int32_t Size;
	void    *Msg;
} PROC_MSG, *PPROC_MSG;

#define	MESSAGE_SIZE8	(sizeof(MESSAGE))
#define	MESSAGE_SIZE16	(sizeof(MESSAGE)/2)
#define	MESSAGE_SIZE32	(sizeof(MESSAGE)/4)

#endif	/* _MSGTYPEDEF_H_ */
