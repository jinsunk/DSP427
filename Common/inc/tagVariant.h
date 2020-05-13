#ifndef _TAGVAR_H_
#define _TAGVAR_H_

#ifndef	_STDINT
#include <stdint.h>
#endif	// _STDINT


#define	VT_EMPTY	(uint16_t)0		/* A property with a type indicator of VT_EMPTY has no data associated with it; that is, the size of the value is zero. */
#define	VT_NULL		(uint16_t)1		/* NULL pointer reference. */
#define	VT_I2		(uint16_t)2		/* Two bytes representing a 2-byte signed integer value. */
#define	VT_I4		(uint16_t)3		/* 4-byte signed integer value  */
#define	VT_R4		(uint16_t)4		/* 32-bit IEEE floating point value. */
#define	VT_R8		(uint16_t)5		/* 64-bit IEEE floating point value. */
#define	VT_CY		(uint16_t)6		/* 8-byte two's complement integer (scaled by 10,000). */
#define	VT_DATE		(uint16_t)7		/* A 64-bit floating point number representing the number of days (not seconds) since December 31, 1899. For example, January 1, 1900, is 2.0, January 2, 1900, is 3.0, and so on). */
									/* This is stored in the same representation as VT_R8.  */
#define	VT_BSTR		(uint16_t)8		/* Pointer to a null-terminated Unicode string. */
#define	VT_DISPATCH	(uint16_t)9		/* A pointer to a pointer to an object was specified. */
#define	VT_ERROR	(uint16_t)10	/* Error code; containing the status code associated with the error. */
#define	VT_BOOL		(uint16_t)11	/* Boolean value. */
#define	VT_VARIANT	(uint16_t)12	/* A type indicator followed by the corresponding value. VT_VARIANT can be used only with VT_BYREF. */
#define	VT_UNKNOWN	(uint16_t)13	/* A pointer to an object that implements the IUnknown interface. */

#define	VT_DECIMAL	(uint16_t)14	/* A decimal structure. */

#define	VT_I1		(uint16_t)16	/* 1-byte signed integer. */
#define	VT_UI1		(uint16_t)17	/* 1-byte unsigned integer. */
#define	VT_UI2		(uint16_t)18	/* 2-byte unsigned integer. */
#define	VT_UI4		(uint16_t)19	/* 4-byte unsigned integer. */

#define	VT_INT		(uint16_t)22	/* 4-byte signed integer value(equivalent to VT_I4). */
#define	VT_UINT		(uint16_t)23	/* 4-byte unsigned integer value(equivalent to VT_UI4). */
#define	VT_VECTOR	(uint16_t)0x1000
#define	VT_ARRAY	(uint16_t)0x2000/* If the type indicator is combined with VT_ARRAY by an OR operator, */
									/* the value is a pointer to a SAFEARRAY. */
									/* VT_ARRAY can use the OR with the following data types: */
									/*		VT_I1, VT_UI1, VT_I2, VT_UI2, VT_I4, VT_UI4, VT_INT, VT_UINT, */
									/*		VT_R4, VT_R8, VT_BOOL, VT_DECIMAL, VT_ERROR, VT_CY, VT_DATE, */
									/*		VT_BSTR, VT_DISPATCH, VT_UNKNOWN and VT_VARIANT. */

#define	VT_BYREF	(uint16_t)0x4000/* If the type indicator is combined with VT_BYREF by an OR operator, */
									/* the value is a reference. Reference types are interpreted as a reference */
									/* to data, similar to the reference type in C++. */


typedef struct tagVARIANT {
	uint16_t vt;

	uint16_t base;
	uint16_t offset;

	uint16_t z;

	union {
		int64_t		llVal;
		int32_t		lVal;
		int8_t		bVal;
		int16_t		iVal;

#ifdef	_MATH_H
		float		fltVal;
		float		*pfltVal;
		double		dblVal;
		double		*pdblVal;
#endif
		char		cVal;
		uint16_t	uiVal;

		uint32_t	ulVal;
		uint64_t	ullVal;
		uint16_t	uintVal;

		uint8_t		ip[8];

		char		*pcVal;

		uint16_t	*puiVal;
		uint32_t	*pulVal;
		uint64_t	*pullVal;

		struct {
			uint32_t	tv_sec;		/* seconds */
			uint32_t	tv_usec;	/* microseconds */
		} time;
	} vd;
} VARIANT, *LPVARIANT, VARIANTARG, *LPVARIANTARG;

#endif	// _TAGVAR_H_
