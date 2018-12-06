#include "unix.h"
#include <fs/proto.h>
#include <errno.h>
#include <fs/dirent.h>

ssize_t readdir(int fd, DIRENT *ent)
{
	if (!FD_INRANGE(fd))
		return -ENFILE;
	if (!svs[fd])
		return -EBADFD;
	return fs_read(svs[fd], ent, size);
}
