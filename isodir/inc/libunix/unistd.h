#pragma once

#define FILEMAX 128

int open(const char *path, int oflags);
int openat(int fd, const char *path, int oflags);
ssize_t read(int fd, char *buf, size_t size);
ssize_t write(int fd, const char *buf, size_t size);
int close(int fd);
