// vim: fdm=marker
#include <types.h>
#include <stddef.h>
#include <string.h>
#include <sysc4.h>
#include "fsproto.h"

#define SERVER 0
#define CLIENT 1
#define TRADER 2

int open(const char *name)
{
	FS_ARGS args;
	args.sysnr = FS_OPEN;
	strncpy(args.name, name, sizeof(args.name) - 1);
	c4_ipc(SERVER, &args);
	return args.fd;
}

long read(int fd, const char *buf)
{
	FS_ARGS args;
	args.sysnr = FS_READ;
	c4_ipc(SERVER, &args);
	return args.rw_size;
}

void my_client(void)
{
	int fd = open("hello.txt");
}

void main(void)
{
	UT_REGS reg;
	if (!c4_fork(CLIENT, &reg))
	{ // CLIENT
		my_client();
	}
	else
	{ // SERVER
		extern void ramfs_server(void);
		ramfs_server();
	}
}
