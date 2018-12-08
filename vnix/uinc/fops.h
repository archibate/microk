#pragma once

#include <inttypes.h>

struct direntry;
int readdir(int fd, struct direntry *ent);
ssize_t read(int fd, void *buf, size_t size);
ssize_t write(int fd, const void *buf, size_t size);
ssize_t pread(int fd, void *buf, size_t size, off_t offset);
ssize_t pwrite(int fd, const void *buf, size_t size, off_t offset);
soff_t lseek(int fd, soff_t offset, int whence);
int close(int fd);
