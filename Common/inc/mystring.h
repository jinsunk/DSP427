#ifndef _MYSTRING_H_
#define _MYSTRING_H_


#ifndef _STDINT
#include <stdint.h>
#endif

#define	COMMAND_NONE		0
#define	COMMAND_CANCEL		1
#define	COMMAND_ENTER		2
#define	COMMAND_PREVIOUS	3
#define	COMMAND_NEXT		4

#define	CMDMODE_ECHO		0	// 문자를 리턴한지 않는다
#define	CMDMODE_NORMAL		1	// 수신한 문자를 리천한다
#define	CMDMODE_PASSWORD	2	// 리턴할 문자를 *로 치환한다


#define	MAX_STRING	256

typedef struct {
	char buff[MAX_STRING];
	int32_t	 length;
	int32_t	 pos;
	int32_t  mode;	// 1 = hidden, return '*' instead of char
	int32_t  echo;

	char ret_str[MAX_STRING];
	int32_t  ret_len;
} CMDSTRING, *PCMDSTRING;

#ifdef __cplusplus
extern "C" {
#endif

extern int32_t input_strcmd(PCMDSTRING strcmd, int n, char *new_str);
extern void clear_strcmd(PCMDSTRING strcmd);
extern int32_t getArguements(char *argv[], char *str);

#ifdef __cplusplus
}
#endif


#endif  // _MYSTRING_H_

