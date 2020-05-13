#ifndef	_SYSMSG_H_
#define	_SYSMSG_H_

	#ifndef	_MSGTYPEDEF_H_
	#include "msgtypedef.h"
	#endif

	#ifndef	__INTRINSICS_INCLUDED
	#include "intrinsics.h"	// for __ramfunc
	#endif


#define	SM_INITIALIZED	((int32_t)(0x0001))
#define	SM_CONNECTED	((int32_t)(0x0002))
#define	SM_ESTABLISHED	((int32_t)(0x0003))
#define	SM_RECEIVED		((int32_t)(0x0004))
#define	SM_TRANSMITTED	((int32_t)(0x0005))
#define	SM_TIMEOUT		((int32_t)(0x0006))
#define	SM_HALFCLOSED	((int32_t)(0x0007))
#define	SM_CLOSED		((int32_t)(0x0008))
#define	SM_CHANGED		((int32_t)(0x0009))
#define	SM_QUIT			((int32_t)(0x000a))
#define	SM_CANCELED		((int32_t)(0x000b))
#define	SM_CFGCHANGED	((int32_t)(0x000c))

#define	SM_ERROR		((int32_t)(0x000f))

#define SM_CHILD_CREATED ((int32_t)(0x0010))
#define SM_CHILD_CLOSED ((int32_t)(0x0011))
#define SM_CHILD_CHANGED ((int32_t)(0x0012))	// what has changed?

#define SM_CHILD_READCOMPLETED ((int32_t)(0x0013))
#define SM_CHILD_WRITECOMPLETED ((int32_t)(0x0014))

#define	SM_READ_COMPLETED	((int32_t)(0x0104))
#define	SM_WRITE_COMPLETED	((int32_t)(0x0105))
#define SM_ERASE_COMPLETED	((int32_t)(0x0106))
#define SM_VERIFY_COMPLETED	((int32_t)(0x0107))

#define	SM_BIT_SET			((int32_t)(0x0120))
#define	SM_BIT_CLEAR		((int32_t)(0x0121))

#define	SM_REBOOT			((int32_t)(0x6FFF))

// local message�� �������� ����ϴ� msg�� �ٸ��� �� �׸������� �ۿ��ϹǷ�
// ���� �׸��� �ٸ��ٸ� �޼����� ���� ���Ƶ� �浹���� �ʴ´�

// ���������� ����ϴ� Timer Message 0x7000 ~ 0x7ffff���� ����� �� �ִ�.
#define	TM_HWRESET		((int32_t)(0x6001))
#define	TM_HWSET		((int32_t)(0x6002))
#define	TM_SWRESET		((int32_t)(0x6003))
#define	TM_SWSET		((int32_t)(0x6004))
#define	TM_INIT			((int32_t)(0x6005))
#define	TM_CONFIG		((int32_t)(0x6006))
#define	TM_RUN			((int32_t)(0x6007))
#define	TM_TEST			((int32_t)(0x6008))
#define	TM_REBOOT		((int32_t)(0x6FFF))	// SM_REBOOT�� ���� ���� ����Ѵ�

// Local���� ����ϴ� Timer Message 0x7000 ~ 0x7ffff���� ����� �� �ִ�.
#define	LM_TIMER		((int32_t)(0x7000))

// Local ���� ����ϴ� Message�� 0x8000 ~ 0x8ffff���� ����� �� �ִ�.
#define	LM_USER			((int32_t)(0x8000))

#ifdef __cplusplus
extern "C" {
#endif

extern void InitSystemMessage(void);
extern uint8_t GetSystemMessage(PMESSAGE);
extern uint8_t SendSystemMessage(callbackmsg_t hprc, int32_t msg, int32_t param);

#ifdef __cplusplus
}
#endif


#endif	// _SYSMSG_H_
