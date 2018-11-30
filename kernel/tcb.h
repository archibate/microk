#pragma once

#include <struct.h>

STRUCT(TCB)
{
	ulong pgd;
#define RUNNING 1
#define BLOCKED 2
	uint state;
	clock_t timeout;
};
