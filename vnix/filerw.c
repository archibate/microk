#include "unix.h"
#include <fs/proto.h>
#include <errno.h>
#include <l4/l4api.h>

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

ich_t getich(int fd)
{
	if (!FD_INRANGE(fd))
		return -ENFILE;
	if (!svs[fd])
		return -EBADFD;
	return fi_getich(svs[fd]);
}

soff_t lseek(int fd, soff_t offset, int whence)
{
	if (!FD_INRANGE(fd))
		return -ENFILE;
	if (!svs[fd])
		return -EBADFD;
	return fi_lseek(svs[fd], offset, whence);
}
