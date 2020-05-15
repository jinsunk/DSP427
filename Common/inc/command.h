#ifndef _COMMAND_H_
#define _COMMAND_H_

	#ifndef _STDINT
	#include <stdint.h>
	#endif

	#ifndef	_MSGTYPEDEF_H_
	#include "msgtypedef.h"
	#endif

typedef void (*cmd_entry_f)(int, char*[]);

typedef struct {
	char			cmdstr[16];
	cmd_entry_f		entryf;
//	int32_t			length;
} COMMAND, *PCOMMAND;


extern void *pDynamicCmdVariable;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif  // _COMMAND_H_


