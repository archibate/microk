#pragma once

#include <l4/defs.h>
#include <struct.h>
#include "caps.h"

UNION(MSGDAT)
{
	struct { ulong di, si, bp, unused_sp, bx, dx; };
	ulong raw[6];
	CAP cap;
};

STRUCT(MSG)
{
	l4id_t tid : 27;
	uint stage : 4;
	uint flags : 2;
	uint is_valid : 1;
	MSGDAT dat;
};
