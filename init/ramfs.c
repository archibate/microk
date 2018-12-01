#include <sysc4.h>
#include <memory.h>
#include "fsproto.h"
#define CLIENT 1

static void myfile_readtx(char *buf);

void myfile_server(void)
{
	FOP_ARGS args;
	while (1)
	{
		c4_wait(CLIENT, &args);
		switch (args.fopnr) {
		case FOP_READTX: myfile_readtx(args.tx_buf);  break;
		default: break;
		};
		c4_send(CLIENT, &args);
	}
}

const char *contents = "Hello, World!";

void myfile_readtx(char *buf)
{
	memcpy(buf, contents, 16);
}
