#pragma once

#include <libl4/l4/defs.h>
#include <types.h>

STRUCT(LWR_CLI)
{
	l4id_t cli;
	size_t size;
	off_t voff;
	off_t mpos;
	cap_t cap;
};

STRUCT(LWR_MSG)
{
	size_t size;
	ushort voff; // < L4_PGSIZ
};

l4id_t l4_lwrsvr_accept(LWR_CLI *lwr);
const void *l4_lwrsvr_romapfg(LWR_CLI *lwr, size_t *psize);
void l4_lwrsvr_release(LWR_CLI *lwr, ssize_t ret);
