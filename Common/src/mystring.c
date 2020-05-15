#include <stdarg.h>
#include <string.h>
#include <ctype.h>		// isprintf
#include <stdlib.h>		// strtol, strtoul, strtoul
#include "mystring.h"

// ������ �͹̳��� VT100�� ȣȯ���� ������ �Ϻ� ���� �ȵǴ� ������ ����
// �� ���α׷��� VT100�� �������� �ۼ��Ͽ���
// Tera Term�� ����� ���� �����մϴ�.
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

	// ������ string�� ���̸� �ʱ�ȭ �Ѵ�
	strcmd->ret_len = 0;

	while(n--)
	{
		newchar = *new_str++;

		switch( newchar )
		{
			case KEY_ESC : // esc code, cntl-c, cntl-z
				if ( n == 0 )
				{	// string�� �����Ͱ� �����ϸ� cancel
					if ( strcmd->length )
						return COMMAND_CANCEL;
					else	// ��Ʈ���� �������� ������ ����
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
							// ���� ����(0x7e)�� �����
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
							// ���� ����(0x7e)�� �����
							n = 0;
							break;

						case KEY_INSERT :
							// ���� ����(0x7e)�� �����
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
						{	// ���ڿ� �߰��� pos�� ������ ��
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
						// cmd �迭�� ������ �����Ѵ�
						strncpy(&strcmd->buff[strcmd->pos], &strcmd->buff[strcmd->pos+1],  diff-1);
						strcmd->length--;
						strcmd->buff[strcmd->length] = '\0';

						// ��� console�� ����� �����͸� �����Ѵ�
						strncpy(&strcmd->ret_str[strcmd->ret_len], &strcmd->buff[strcmd->pos], diff-1);
						strcmd->ret_len += (diff-1);
						strcmd->ret_str[strcmd->ret_len++] = ' ';	// ������ ���ڸ� �����

						while(diff--)
							strcmd->ret_str[strcmd->ret_len++] = 0x08;
					}
				}
				break;

			case 0x0d:	// return key
				// return strcmd
				strcmd->ret_str[strcmd->ret_len++] = '\r';
				strcmd->ret_str[strcmd->ret_len++] = '\n';

				// �������� ���� ���� ����,
				strcmd->buff[strcmd->length] = '\0';
				return COMMAND_ENTER;

			default :
				if ( strcmd->length < (MAX_STRING-1) )
				{
					if ( isprint( newchar) )
					{
						int diff = strcmd->length - strcmd->pos;

						// ���� ������ ���ڸ� ������ ���ۿ� �����Ѵ�
						strcmd->ret_str[strcmd->ret_len++] = newchar;

						if ( diff )
						{	// pos�� ���ڿ��� �߰����� insert�� �� ��
							// ������ pos�� ��ġ�� �ִ� ���ڿ��� �ڷ� �����Ѵ�
							strncpy(&strcmd->ret_str[strcmd->ret_len], &strcmd->buff[strcmd->pos], diff);
							strcmd->ret_len += diff;
						}

						strncpy(&strcmd->buff[strcmd->pos], strcmd->ret_str,  strcmd->ret_len);

						while(diff--)
							strcmd->ret_str[strcmd->ret_len++] = 0x08;

						strcmd->pos++;
						strcmd->length++;

						// ������ ���ڸ� ��带 ���� ��ȯ�Ѵ�
						if (strcmd->mode)
						{
							int i;	// hidden�̸� ��� ���ڸ� *�� �ٲ۴�
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
		argc++;		// ���ڸ� ���� ��Ų��

		while( argv[argc] = strtok( NULL, " ") )
		{
			argc++;		// ���ڸ� ���� ��Ų��
		}
	}

	return argc;
}

