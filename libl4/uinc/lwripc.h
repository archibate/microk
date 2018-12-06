#pragma once

#include <libl4/l4/defs.h>

L4_MSG_STRUCT(LWR_MSG)
{
	uint fsop;
	uint voff;
	uint size;
}
END_L4_MSG_STRUCT;

STRUCT(LWR_CLI)
{
	l4id_t cli;
	uint voff;
	uint size;
};

ssize_t l4_lwrite(l4id_t to, const void *buf, size_t size);
ssize_t l4_lwread_ex(l4id_t fr, void *buf, size_t size, l4id_t *pfr);
#define l4_lwread(fr, buf, size) l4_lwread_ex(fr, buf, size, NULL)
l4id_t l4_lwrpga_accept(LWR_CLI *lwr, l4id_t fr);
const void *l4_lwrpga_getptr(LWR_CLI *lwr);
int l4_lwrpga_reply(LWR_CLI *lwr, long ret);
