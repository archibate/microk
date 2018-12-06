#include <fs/proto.h>
#include <string.h>
#include <libl4/rwipc.h>
#include <errno.h>

l4id_t fs_psvopensvr(l4id_t pathsvr, const char *path, uint oflags)
{
	size_t size = strlen(path);
	if (size > PATHMAX)
		return -ENAMETOOLONG;
       	ssize_t wsize = l4_write(pathsvr, path, size);
	if (wsize < 0)
		return wsize;
	if (wsize < size) {
		l4_puts("WARNNING: fs_psvopensvr: result of l4_write to pathsvr infering ENAMETOOLONG");
		return -ENAMETOOLONG;
	}
	int ret = l4_sendich(pathsvr, 13, oflags);
	if (ret < 0)
		return ret;
	l4id_t svr = l4_recvich(pathsvr, 2);
	return svr;
}
