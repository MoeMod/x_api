#pragma once

typedef struct
{
	int (*buildnum)(void);
}hook_funcs_t;

extern hook_funcs_t gHookFuncs;
