#ifndef	_SYSCNTL_H_
#define	_SYSCNTL_H_


	#ifndef _STDINT
	#include <stdint.h>
	#endif

#define	DIAG_RAM_ERR	((uint16_t)0x0001)
#define	DIAG_ROM_ERR	((uint16_t)0x0002)


#ifdef __cplusplus
extern "C" {
#endif

extern void syscntl_ProcessInit(void);

#ifdef __cplusplus
}
#endif


#endif	// _SYSCNTL_H_
