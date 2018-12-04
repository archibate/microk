#include <libl4/ichipc.h>
#include <libl4/rwipc.h>
#include <pathsvr.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define PATHSVR 1

static l4id_t getsvrof(const char *path)
{
	l4_write(PATHSVR, path, strlen(path));
	l4id_t svr = l4_recvich(PATHSVR);
	return svr;
}

static l4id_t svs[FILEMAX];

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

int openat(int fd, const char *path, int oflags)
{
	if (fd < 0 || fd >= FILEMAX)
		return -ENFILE;
	if (svs[fd])
		close(fd);
	l4id_t svr = getsvrof(path);
	if (svr < 0)
		return svr;
	svs[fd] = svr;
	return fd;
}

int close(int fd)
{
	if (fd < 0 || fd >= FILEMAX)
		return -ENFILE;
	if (!svs[fd])
		return -EBADFD;
	l4_sendich(svs[fd], FILE_CLOSE);
	svs[fd] = 0;
	return 0;
}

ssize_t read(int fd, char *buf, size_t size)
{
	if (fd < 0 || fd >= FILEMAX)
		return -ENFILE;
	if (!svs[fd])
		return -EBADFD;
	l4_sendich(svs[fd], FILE_READ);
	return l4_read(svs[fd], buf, size);
}

ssize_t write(int fd, const char *buf, size_t size)
{
	if (fd < 0 || fd >= FILEMAX)
		return -ENFILE;
	if (!svs[fd])
		return -EBADFD;
	l4_sendich(svs[fd], FILE_WRITE);
	return l4_write(svs[fd], buf, size);
}
