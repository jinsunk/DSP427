#ifndef _CONSOLE_H_
#define _CONSOLE_H_


// typedef int32_t (*transfer_f)(uint8_t*, int32_t);


	#ifndef	_STDINT
	#include <stdint.h>
	#endif	// _STDINT

	#ifndef	_MSGTYPEDEF_H_
	#include "msgtypedef.h"
	#endif

	#ifndef	_LIST_H_
	#include "list.h"
	#endif

	#ifndef	_COMMAND_H_
	#include "command.h"
	#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void console_ProcessInit(void);
extern void console_RegisterMsgHandle(callbackmsg_t);

extern int32_t console_Send(char *pbuff, int32_t size);
extern int32_t console_Recv(uint8_t *pbuff, int32_t maxsize);
extern int32_t console_printf(const char *format, ...);
extern int32_t console_AddCommand(PLIST pList, PCOMMAND pCommand);

#ifdef __cplusplus
}
#endif

#endif

