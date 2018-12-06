#include "unix.h"
#include <fs/proto.h>
#include <errno.h>

l4id_t svs[FILEMAX];

int allocfd(void)
{
	for (int i = 0; i < FILEMAX; i++)
		if (!svs[i])
			return i;
	return -1;
}

int open(const char *path, int oflags)
{
	int fd = allocfd();
	if (fd == -1)
		return -EMFILE;
	return openat(fd, path, oflags);
}

int opensvr(l4id_t svr, int oflags)
{
	int fd = allocfd();
	if (fd == -1)
		return -EMFILE;
	return opensvrat(fd, svr, oflags);
}

int openat(int fd, const char *path, int oflags)
{
	if (!FD_INRANGE(fd))
		return -ENFILE;
	if (svs[fd])
		close(fd);
	l4id_t svr = fs_opensvr(path, oflags);
	if (svr < 0)
		return svr;
	svs[fd] = svr;
	return fd;
}

int opensvrat(int fd, l4id_t svr, int oflags)
{
	if (!FD_INRANGE(fd))
		return -ENFILE;
	if (svs[fd])
		close(fd);
	svs[fd] = svr;
	return fd;
}

int close(int fd)
{
	if (!FD_INRANGE(fd))
		return -ENFILE;
	if (!svs[fd])
		return -EBADFD;
	fs_close(svs[fd]);
	svs[fd] = 0;
	return 0;
}
