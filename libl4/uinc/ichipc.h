#pragma once

#include <libl4/l4/api.h>
#include <libl4/ipcmsgs.h>

static int l4_sendich_ex(l4id_t to, uint ich, int flags)
{
	ICH_MSG icmsg;
	icmsg.ich = ich;
	return l4_send_ex(to, &icmsg, flags);
}

#define l4_sendich(to, ich) l4_sendich_ex(to, ich, L4_BLOCK)

static uint l4_recvich_ex(l4id_t fr, l4id_t *pfr)
{
	ICH_MSG icmsg;
	fr = l4_recv(fr, &icmsg);
	if (pfr)
		*pfr = fr;
	return icmsg.ich;
}

#define l4_recvich(to) l4_recvich_ex(to, NULL)

#if 0
ssize_t l4_sendtx(l4id_t to, const void *buf, size_t size);
ssize_t l4_recvtx(l4id_t fr, void *buf, size_t size, l4id_t *pfr);
ssize_t l4_recvtx_least(l4id_t fr, void *buf, size_t size, l4id_t *pfr);
static ssize_t l4_recvtx_most(l4id_t fr, void *buf, size_t size, l4id_t *pfr)
{
	if (size < L4_FRGSIZ)
		return 0;
	return l4_recvtx_least(fr, pfr, buf, size - L4_FRGSIZ);
}
#endif
