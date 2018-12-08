#pragma once

#include <fs/fsdefs.h> // O_*

#define FILEMAX 128

int open(const char *path, int oflags);
int openat(int fd, const char *path, int oflags);
