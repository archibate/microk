#pragma once

#include <l4/defs.h>
#include <struct.h>

UNION(MSGDAT)
{
	struct { ulong di, si, bp, unused_sp, bx, dx; };
	char raw[24];
};

STRUCT(MSG)
{
	l4id_t tid : 30;
	uint is_async : 1;
	uint is_valid : 1;
	MSGDAT dat;
};
