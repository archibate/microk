#pragma once

#include <types.h>
#include <struct.h>
STRUCT(MEMREG_CAP)
{
	ulong base;
	ulong size;
	uint rwx;
};

#include "tcb.h"
STRUCT(THREAD_CAP)
{
	TCB *tcb;
	uint type;
};

STRUCT(CAP)
{
	union {
		uint raw[4];
		THREAD_CAP thread_cap;
		MEMREG_CAP memreg_cap;
	};
};

#define VALID_CAP(cap) ((cap)->raw[0])
