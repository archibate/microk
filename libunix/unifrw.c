#include "libunix.h"
#include <libl4/ichipc.h>
#include <libl4/lwripc.h>
#include <libl4/rwipc.h>
#include <pathsvr.h>
#include <string.h>
#include <errno.h>

static int need_large(const void *buf, size_t size)
{
	if (size > L4_FRGSIZ * 2)
		return 1;
	else
		return 0;
}

ssize_t read(int fd, void *buf, size_t size)
{
	if (fd < 0 || fd >= FILEMAX)
		return -ENFILE;
	if (!svs[fd])
		return -EBADFD;
	int large = need_large(buf, size), tried = 0;
again:
	l4_sendich(svs[fd], 1, large ? FILE_LREAD : FILE_READ);
	ssize_t ret = large ? l4_lwread(svs[fd], buf, size) : l4_read(svs[fd], buf, size);
	if (!tried && ret == -ENOSYS) {
		large = !large;
		tried = 1;
		goto again;
	}
	return ret;
}

ssize_t write(int fd, const void *buf, size_t size)
{
	if (fd < 0 || fd >= FILEMAX)
		return -ENFILE;
	if (!svs[fd])
		return -EBADFD;
	int large = need_large(buf, size), tried = 0;
again:
	l4_sendich(svs[fd], 1, large ? FILE_LWRITE : FILE_WRITE);
	ssize_t ret = large ? l4_lwrite(svs[fd], buf, size) : l4_write(svs[fd], buf, size);
	if (!tried && ret == -ENOSYS) {
		large = !large;
		tried = 1;
		goto again;
	}
	return ret;
}
