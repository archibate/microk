#pragma once

#include <l4/defs.h>
#include <struct.h>
#include "caps.h"

UNION(MSGDAT)
{
	struct { ulong di, si, bp, unused_sp, bx, dx; };
	char raw[24];
	CAP cap;
};

STRUCT(MSG)
{
	l4id_t tid : 29;
	uint flags : 2;
	uint is_valid : 1;
	MSGDAT dat;
};
