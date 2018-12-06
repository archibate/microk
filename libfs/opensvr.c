#include <fs/proto.h>
#include <fs/opensvr.h>
#include <l4/rwipc.h>
#include <l4/ichipc.h>
#include <string.h>
#include <errno.h>
#include <debug.h>

l4id_t fi_psvopensvr(l4id_t pathsvr, const char *path, fotype_t oflags)
{
	size_t size = strlen(path);
	if (size > PATHMAX)
		return -ENAMETOOLONG;
       	ssize_t wsize = l4_write(pathsvr, path, size);
	if (wsize < 0)
		return wsize;
	if (wsize < size) {
		dbg_printf("WARNNING: fs_psvopensvr: result of l4_write to pathsvr infering ENAMETOOLONG\n");
		return -ENAMETOOLONG;
	}
	int ret = l4_sendich(pathsvr, 13, oflags);
	if (ret < 0)
		return ret;
	l4id_t svr = l4_recvich(pathsvr, 2);
	return svr;
}
