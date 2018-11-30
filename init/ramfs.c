#include <sysc4.h>
#include "fsproto.h"
#define CLIENT 1

int ramfs_open(const char *name);
long ramfs_read(int fd);

void ramfs_server(void)
{
	FS_ARGS args;
	while (1)
	{
		c4_ipc(CLIENT, &args);
		switch (args.sysnr) {
		case FS_OPEN:  args.ret_fd  = ramfs_open(args.name);    break;
		case FS_READ:  args.rw_size = ramfs_read(args.arg_fd);  break;
		};
	}
}

int ramfs_open(const char *name)
{
}

long ramfs_read(int fd)
{
}
