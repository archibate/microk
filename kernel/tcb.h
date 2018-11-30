#pragma once

#include <struct.h>
// fpage and aspace: will be removed {{{
STRUCT(FPAGE)
{
	FPAGE *next;
	ulong va, size, pte;
};

STRUCT(SPACE)
{
	FPAGE *fpg0;
};
// }}}
STRUCT(TCB)
{
	ulong pgd;
#define RUNNING 1
#define BLOCKED 2
	uint state;
	uint timeout;
	SPACE as;
};
