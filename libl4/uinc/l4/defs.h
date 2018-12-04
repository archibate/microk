#pragma once

#include <krnl4/l4/defs.h>
#include <struct.h>

UNION(L4_MSG) // short message, exactly 20 bytes
{
	struct {
		ulong dx, si, di, bp, bx;
	};
	ulong regs[5];
};
#define L4_FRGSIZ sizeof(L4_MSG)
