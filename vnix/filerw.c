#include "unix.h"
#include <fs/proto.h>
#include <errno.h>

ssize_t read(int fd, void *buf, size_t size)
{
	if (!FD_INRANGE(fd))
		return -ENFILE;
	if (!svs[fd])
		return -EBADFD;
	return fi_read(svs[fd], buf, size);
}

ssize_t write(int fd, const void *buf, size_t size)
{
	if (!FD_INRANGE(fd))
		return -ENFILE;
	if (!svs[fd])
		return -EBADFD;
	return fi_write(svs[fd], buf, size);
}
