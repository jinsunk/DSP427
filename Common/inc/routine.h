#ifndef	_ROUTINE_H_
#define	_ROUTINE_H_

	#ifndef _STDINT
	#include <stdint.h>
	#endif

#define	RT_CYCLIC	1	// �ѹ� ������ �� �ݵ�� ȣ��Ǿ�� �ϴ� �Լ�
#define	RT_PERIODIC	2	// ���������� ����Ǿ �Ǵ� �Լ�

typedef int32_t (*routinef_t)(void);


#ifdef __cplusplus
extern "C" {
#endif


extern void InitRoutine(void);
extern void RunRoutine(void);
extern int32_t InsertRoutine(int32_t rt_type, routinef_t rt_func);

#ifdef __cplusplus
}
#endif


#endif	// _ROUTINE_H_
