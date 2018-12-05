#pragma once

#define FILEMAX 128

int open(const char *path, int oflags);
int openat(int fd, const char *path, int oflags);
#ifdef __L4
int opensvr(l4id_t svr, int oflags);
int opensvrat(int fd, l4id_t svr, int oflags);
int closesvr(int fd);
#endif
ssize_t read(int fd, void *buf, size_t size);
ssize_t write(int fd, const void *buf, size_t size);
int close(int fd);
