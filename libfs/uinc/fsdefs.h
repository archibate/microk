#pragma once

#include <inttypes.h>

#define PATHMAX 512
#define NAMEMAX 256

#define FS_READ   1
#define FS_WRITE  2
#define FS_SREAD  3
#define FS_SWRITE 4
#define FS_CLOSE  5
#define FS_LSEEK  6
#define FS_GETICH 7
typedef int fscmd_t;

#define O_RDONLY 1
#define O_WRONLY 2
#define O_RDWR   3
#define O_EXEC   4
#define O_CREAT  8
typedef int fotype_t;

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#ifndef EOF
#define EOF (-1)
#endif
typedef uint ich_t;
