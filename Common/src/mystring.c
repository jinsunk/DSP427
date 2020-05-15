#include <stdarg.h>
#include <string.h>
#include <ctype.h>		// isprintf
#include <stdlib.h>		// strtol, strtoul, strtoul
#include "mystring.h"

// 하이퍼 터미널이 VT100과 호환성이 떨어져 일부 동작 안되는 문제가 있음
// 이 프로그램은 VT100을 기준으로 작성하였음
// Tera Term을 사용할 것을 권장합니다.
// Key values

// Home		:	(0x1b 5b 31 7e)
// Insert	:	(0x1b 5b 32 7e)
// End		:	(0x1b 5b 34 7e)
// PgUp		:	(0x1b 5b 35 7e)
// PgDown	:	(0x1b 5b 36 7e)

// Up		:	(0x1b 5b 41)
// Down		:	(0x1b 5b 42)
// Right	:	(0x1b 5b 43)
// Left		:	(0x1b 5b 44)

#define	KEY_BACK	(0x08)
#define	KEY_DEL		(0x7f)

#define	KEY_ESC		(0x1b)

#define	KEY_HOME	(0x31)
#define	KEY_INSERT	(0x32)
#define	KEY_END		(0x34)
#define	KEY_PGUP	(0x35)
#define	KEY_PGDN	(0x36)

#define	KEY_UP		(0x41)
#define	KEY_DOWN	(0x42)
#define	KEY_RIGHT	(0x43)
#define	KEY_LEFT	(0x44)

#define	KEY_CNTLC	(0x03)
#define	KEY_CNTLZ	(0x1a)

void clear_strcmd(PCMDSTRING strcmd)
{
	strcmd->length = 0;
	strcmd->pos = 0;
	strcmd->ret_len = 0;
}

int32_t input_strcmd(PCMDSTRING strcmd, int n, char *new_str)
{
	char newchar;

	// 돌려줄 string의 길이를 초기화 한다
	strcmd->ret_len = 0;

	while(n--)
	{
		newchar = *new_str++;

		switch( newchar )
		{
			case KEY_ESC : // esc code, cntl-c, cntl-z
				if ( n == 0 )
				{	// string에 데이터가 존재하면 cancel
					if ( strcmd->length )
						return COMMAND_CANCEL;
					else	// 스트링이 존재하지 않으면 무시
						break;	//  break while
				}

				n--;
				newchar = *new_str++;

				if ( newchar == '[' )
				{
					if ( n == 0 ) break;

					n--;
					newchar = *new_str++;

					switch ( newchar )
					{
						case KEY_RIGHT :	// right key
							if ( strcmd->pos < strcmd->length)
							{
								strcmd->pos++;

								if ( strcmd->echo )
								{
									strcmd->ret_str[strcmd->ret_len++] = KEY_ESC;
									strcmd->ret_str[strcmd->ret_len++] = '[';
									strcmd->ret_str[strcmd->ret_len++] = KEY_RIGHT;
								}

								// return strcmd
							}
							break;

						case KEY_LEFT :	// left key
							if ( strcmd->pos > 0)
							{
								strcmd->pos--;

								if ( strcmd->echo )
								{
									//strcmd->ret_str[strcmd->ret_len++] = KEY_ESC;
									//strcmd->ret_str[strcmd->ret_len++] = '[';
									//strcmd->ret_str[strcmd->ret_len++] = KEY_LEFT;
									strcmd->ret_str[strcmd->ret_len++] = KEY_BACK;
								}
							}
							break;

						case KEY_HOME :
							while( strcmd->pos != 0 )
							{
								strcmd->pos--;

								if ( strcmd->echo )
								{
									strcmd->ret_str[strcmd->ret_len++] = KEY_ESC;
									strcmd->ret_str[strcmd->ret_len++] = '[';
									strcmd->ret_str[strcmd->ret_len++] = KEY_LEFT;
								}
							}
							// 남은 문자(0x7e)를 지운다
							n = 0;
							break;

						case KEY_END :
							while( strcmd->pos != strcmd->length )
							{
								strcmd->pos++;

								if ( strcmd->echo )
								{
									strcmd->ret_str[strcmd->ret_len++] = KEY_ESC;
									strcmd->ret_str[strcmd->ret_len++] = '[';
									strcmd->ret_str[strcmd->ret_len++] = KEY_RIGHT;
								}
							}
							// 남은 문자(0x7e)를 지운다
							n = 0;
							break;

						case KEY_INSERT :
							// 남은 문자(0x7e)를 지운다
							n = 0;
							break;

						case KEY_UP :
							//return COMMAND_CANCEL;

						case KEY_DOWN :
							//return COMMAND_CANCEL;


						default :
							;
					}
				}
				break;

			case KEY_BACK:	// back space key
				if ( strcmd->pos > 0 )
				{
					if (strcmd->length > 0)
					{
						int diff = 0;

						strcmd->ret_str[strcmd->ret_len++] = 0x08;

						if ( strcmd->length != strcmd->pos )
						{	// 문자열 중간에 pos이 존재할 때
							diff = (strcmd->length - strcmd->pos);

							strncpy(&strcmd->ret_str[strcmd->ret_len], &strcmd->buff[strcmd->pos], diff);
							strcmd->ret_len += diff;

							strncpy(&strcmd->buff[strcmd->pos-1], &strcmd->buff[strcmd->pos],  diff);
						}

						strcmd->pos--;
						strcmd->length--;

						strcmd->buff[strcmd->length] = '\0';

						// return strcmd
						strcmd->ret_str[strcmd->ret_len++] = ' ';
						strcmd->ret_str[strcmd->ret_len++] = 0x08;

						while(diff--)
							strcmd->ret_str[strcmd->ret_len++] = 0x08;

					}
				}
				break;

			case KEY_DEL:	// Delete key
				if ( strcmd->pos != strcmd->length )
				{
					int diff = (strcmd->length - strcmd->pos);

					if ( diff )
					{
						// cmd 배열의 내용을 갱신한다
						strncpy(&strcmd->buff[strcmd->pos], &strcmd->buff[strcmd->pos+1],  diff-1);
						strcmd->length--;
						strcmd->buff[strcmd->length] = '\0';

						// 상대 console에 사용할 데이터를 수정한다
						strncpy(&strcmd->ret_str[strcmd->ret_len], &strcmd->buff[strcmd->pos], diff-1);
						strcmd->ret_len += (diff-1);
						strcmd->ret_str[strcmd->ret_len++] = ' ';	// 마지막 글자를 지우고

						while(diff--)
							strcmd->ret_str[strcmd->ret_len++] = 0x08;
					}
				}
				break;

			case 0x0d:	// return key
				// return strcmd
				strcmd->ret_str[strcmd->ret_len++] = '\r';
				strcmd->ret_str[strcmd->ret_len++] = '\n';

				// 마지막에 문자 끝을 쓴다,
				strcmd->buff[strcmd->length] = '\0';
				return COMMAND_ENTER;

			default :
				if ( strcmd->length < (MAX_STRING-1) )
				{
					if ( isprint( newchar) )
					{
						int diff = strcmd->length - strcmd->pos;

						// 새로 수신한 문자를 리턴할 버퍼에 저장한다
						strcmd->ret_str[strcmd->ret_len++] = newchar;

						if ( diff )
						{	// pos이 문자열의 중간에서 insert가 될 때
							// 현재의 pos의 위치에 있는 문자열을 뒤로 대피한다
							strncpy(&strcmd->ret_str[strcmd->ret_len], &strcmd->buff[strcmd->pos], diff);
							strcmd->ret_len += diff;
						}

						strncpy(&strcmd->buff[strcmd->pos], strcmd->ret_str,  strcmd->ret_len);

						while(diff--)
							strcmd->ret_str[strcmd->ret_len++] = 0x08;

						strcmd->pos++;
						strcmd->length++;

						// 리턴할 문자를 모드를 보고 변환한다
						if (strcmd->mode)
						{
							int i;	// hidden이면 모든 문자를 *로 바꾼다
							for(i = 0; i < strcmd->ret_len; i++)
								if ( isprint( strcmd->ret_str[i] ) )
									strcmd->ret_str[i] = '*';
						}
					}
				}
		}
	}

	return COMMAND_NONE;
}

int32_t getArguements(char *argv[], char *argstr)
{
	int32_t argc = 0;

	argv[0] = strtok(argstr, " ");
	if ( argv[0] )
	{
		argc++;		// 인자를 증가 시킨다

		while( argv[argc] = strtok( NULL, " ") )
		{
			argc++;		// 인자를 증가 시킨다
		}
	}

	return argc;
}

