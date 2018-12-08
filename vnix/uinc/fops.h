#pragma once

#include <inttypes.h>

ssize_t read(int fd, void *buf, size_t size);
ssize_t write(int fd, const void *buf, size_t size);
soff_t lseek(int fd, soff_t offset, int whence);
int close(int fd);
