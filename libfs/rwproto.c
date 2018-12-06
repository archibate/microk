#include <fs/proto.h>
#include <libl4/rwipc.h>
#include <libl4/lwripc.h>
#include <errno.h>

static int need_large(const void *buf, size_t size)
{
	if (size > L4_FRGSIZ * 2)
		return 1;
	else
		return 0;
}

ssize_t fs_read(l4id_t svr, void *buf, size_t size)
{
	int large = need_large(buf, size), tried = 0;
again:
	fs_sendcmd(svr, large ? FS_LREAD : FS_READ);
	ssize_t ret = large ? l4_lwread(svr, buf, size) : l4_read(svr, buf, size);
	if (!tried && ret == -ENOSYS) {
		large = !large;
		tried = 1;
		goto again;
	}
	return ret;
}

ssize_t fs_write(l4id_t svr, const void *buf, size_t size)
{
	int large = need_large(buf, size), tried = 0;
again:
	fs_sendcmd(svr, large ? FS_LWRITE : FS_WRITE);
	ssize_t ret = large ? l4_lwrite(svr, buf, size) : l4_write(svr, buf, size);
	if (!tried && ret == -ENOSYS) {
		large = !large;
		tried = 1;
		goto again;
	}
	return ret;
}
