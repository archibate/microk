#pragma once

#include <krnl4/l4/defs.h>
#include <struct.h>

UNION(L4_MSG) // short message, exactly 20 bytes
{
	struct {
		ulong dx, si, di, bp, bx;
	};
	ulong raw[5];
};
#define L4_FRGSIZ sizeof(L4_MSG)

#define L4_MSG_STRUCT(type) UNION(type) { L4_MSG l4msg; struct
#define END_L4_MSG_STRUCT ;}

#define __L4
