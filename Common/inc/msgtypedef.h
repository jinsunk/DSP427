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
	callbackmsg_t	callfunc;	// �޼����� ����� function
	int32_t		message;	// message�� ����
	int32_t		params;		// ���� ����, value �Ǵ� pointer�� �� �ִ�.

#ifdef	__MSG_TIME_STAMP__
	time_t	seconds;			// �����ð�
	time_t	fraction;
#endif
	//int process id ??	// �߻��� message ������ ����ϴ� ID
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
