// vim: fdm=marker
#include <types.h>
#include <stddef.h>
#include <memory.h>
#include <sysc4.h>
#include "fsproto.h"

#define SERVER   1
#define KMEM_CAP 3
#define VRAM_CAP 4

void readtx(int svr, char *buf, size_t size)
{
	FOP_ARGS args;
	args.fopnr = FOP_READTX;
	c4_send(svr, &args);
	c4_wait(svr, &args);
	memcpy(buf, args.tx_buf, sizeof(args.tx_buf));
}

void my_client(void)
{
	static char buf[32];
	readtx(SERVER, buf, sizeof(buf));
	c4_puts(buf);
}

void main(void)
{
	if (!c4_fork(1, NULL))
	{
		c4_real(KMEM_CAP);
		extern void myfile_server(char *ramdisk);
		myfile_server((char*)0xa0007c00);
	}
	else
	{
		c4_actv(1, NULL);//
		my_client();
		c4_halt();
	}
}
