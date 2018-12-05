#pragma once

#include <libl4/l4/api.h>
#include <errno.h>

static int l4_sendcap_ex(l4id_t to, stage_t stg, cap_t capid, uint flags)
{
	return __l4_send(to, stg, NULL, flags | L4_ISCAP, capid);
}
#define l4_sendcap(to, stg, capid) l4_sendcap_ex(to, stg, capid, L4_BLOCK)

static l4id_t l4_recvcap(l4id_t fr, stage_t stg, cap_t capid)
{
	l4_recv_ret_t recv = __l4_recv(fr, stg, NULL, capid);
	if (recv.ret < 0)
		return recv.ret;
	if (!(recv.flags & L4_ISCAP))
		return -ENOTCAP;
	return recv.ret;
}
