#pragma once

#include "fstypes.h"

#define DT_UNKNOWN 0	// The file type could not be determined.
#define DT_REG     1	// This is a regular file.
#define DT_SOCK    2	// This is a UNIX domain socket.
#define DT_BLK     3	// This is a block device.
#define DT_CHR     4	// This is a character device.
#define DT_DIR     5	// This is a directory.
#define DT_FIFO    6	// This is a named pipe (FIFO).
#define DT_LNK     7	// This is a symbolic link.

STRUCT(DIRENT)
{
	ino_t  d_ino;			// Inode number
	ushort d_reclen;		// Length of this record
	uchar  d_type;			// Type of file
	char   d_name[NAMEMAX+1];	// Null terminated file name
};
