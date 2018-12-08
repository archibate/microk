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

ssize_t pread(int fd, void *buf, size_t size, off_t offset)
{
	if (!FD_INRANGE(fd))
		return -ENFILE;
	if (!svs[fd])
		return -EBADFD;
	return fi_pread(svs[fd], buf, size, offset);
}

ssize_t pwrite(int fd, const void *buf, size_t size, off_t offset)
{
	if (!FD_INRANGE(fd))
		return -ENFILE;
	if (!svs[fd])
		return -EBADFD;
	return fi_pwrite(svs[fd], buf, size, offset);
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

int readdir(int fd, struct direntry *ent)
{
	if (!FD_INRANGE(fd))
		return -ENFILE;
	if (!svs[fd])
		return -EBADFD;
	return fi_readdir(svs[fd], ent);
}
