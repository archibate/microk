#include <fs/pathsvr.h>
#include <l4/rwipc.h>
#include <l4/ichipc.h>

ssize_t fspsv_recv_opener(OPENER_ARGS *oa, l4id_t fr)
{
	ssize_t size = l4_read_ex(L4_ANY, oa->path, PATHMAX, &oa->cli);
	if (size < 0)
		return size;
	oa->path[size] = 0;
	oa->oflags = l4_recvich(oa->cli, 13);
	return size;
}

int fspsv_reply_opener(OPENER_ARGS *oa, l4id_t ret_svr)
{
	return l4_sendich_ex(oa->cli, 2, ret_svr, L4_REPLY);
}
