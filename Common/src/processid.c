#include "processid.h"

static void *_pid_table[MAX_PID];

void *pid_GetProcess(int32_t idx)
{
	if ( idx >= MAX_PID ) return ((void*)0);

	return _pid_table[idx];
}

int32_t pid_SetProcess(int32_t idx, void *invoc)
{
	if ( idx >= MAX_PID ) return -1;

	_pid_table[idx] = invoc;
	return 0;
}

void pid_Init(void)
{
	for( int idx = 0; idx < MAX_PID; idx++)
		_pid_table[idx] = (void*)0;
}

