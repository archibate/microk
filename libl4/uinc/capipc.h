#pragma once

#include <libl4/l4/api.h>
#include <errno.h>

static int l4_sendcap_ex(l4id_t to, cap_t capid, uint flags)
{
	return __l4_send(to, NULL, flags | L4_ISCAP, capid);
}
#define l4_sendcap(to, capid) l4_sendcap_ex(to, capid, L4_BLOCK)

static l4id_t l4_recvcap(l4id_t fr, cap_t capid)
{
	l4_recv_ret_t recv = __l4_recv(fr, NULL, capid);
	if (recv.ret < 0)
		return recv.ret;
	if (!(recv.flags & L4_ISCAP))
		return -ENOTCAP;
	return recv.ret;
}
