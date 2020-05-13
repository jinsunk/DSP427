#ifndef	_MEMORY_H_
#define	_MEMORY_H_

#ifndef _STDINT
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void MemoryInit(void);
extern void* CCR_malloc(int32_t);
extern void* SDR_malloc(int32_t);

#ifdef __cplusplus
}
#endif

#endif	/* _MEMORY_H_ */
