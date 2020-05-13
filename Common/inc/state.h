#ifndef	_STATE_H_
#define	_STATE_H_

	#ifndef _STDINT
	#include <stdint.h>
	#endif

#define	ST_IDLE		((int32_t)0)

#define	ST_HWRESET	((int32_t)1)
#define	ST_HWSET	((int32_t)2)

#define	ST_SWRESET	((int32_t)3)
#define	ST_SWSET	((int32_t)4)

#define	ST_INIT		((int32_t)5)
#define	ST_CONFIG	((int32_t)6)

#define	ST_RUN		((int32_t)7)
#define	ST_TEST		((int32_t)8)


#define	ST_LOCAL	((int32_t)0x80)


// return  values
#define	ST_BUSY		((int32_t)-1)
#define	ST_ERROR	((int32_t)-2)
#define	ST_SUCCESS	((int32_t)0)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif


#endif	// _STATE_H_
