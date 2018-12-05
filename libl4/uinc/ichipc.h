#pragma once

#include <libl4/l4/api.h>
#include <libl4/ipcmsgs.h>

static int l4_sendich_ex(l4id_t to, stage_t stg, uint ich, int flags)
{
	ICH_MSG icmsg;
	icmsg.ich = ich;
	return l4_send_ex(to, stg, &icmsg, flags);
}

#define l4_sendich(to, stg, ich) l4_sendich_ex(to, stg, ich, L4_BLOCK)

static uint l4_recvich_ex(l4id_t fr, stage_t stg, l4id_t *pfr)
{
	ICH_MSG icmsg;
	fr = l4_recv(fr, stg, &icmsg);
	if (pfr)
		*pfr = fr;
	return icmsg.ich;
}

#define l4_recvich(fr, stg) l4_recvich_ex(fr, stg, NULL)

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
