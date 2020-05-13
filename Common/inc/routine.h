#ifndef	_ROUTINE_H_
#define	_ROUTINE_H_

	#ifndef _STDINT
	#include <stdint.h>
	#endif

#define	RT_CYCLIC	1	// 한번 수행할 때 반드시 호출되어야 하는 함수
#define	RT_PERIODIC	2	// 간헐적으로 수행되어도 되는 함수

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
