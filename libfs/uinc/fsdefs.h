#pragma once

#define PATHMAX 512
#define NAMEMAX 256

#define FS_READ   1
#define FS_WRITE  2
#define FS_SREAD  3
#define FS_SWRITE 4
#define FS_CLOSE  5
typedef int fscmd_t;

#define O_RDONLY 1
#define O_WRONLY 2
#define O_RDWR   3
#define O_EXEC   4
#define O_CREAT  8
typedef int fotype_t;

#include <types.h>

typedef ulong ino_t;
