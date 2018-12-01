// vim: fdm=marker
#include <types.h>
#include <stddef.h>
#include <memory.h>
#include <c4/api.h>
#include "keysvr.h"

#define SERVER   1
#define KMEM_CAP 3
#define VRAM_CAP 4

int getich(int svr)
{
	TX_ARGS args;
	c4_send(svr, NULL);
	c4_wait(svr, &args);
	return args.tx_ich;
}

void my_client(void)
{
	while (1) {
		uint ich = getich(SERVER);
		c4_print(ich);
	}
}

void main(void)
{
	if (!c4_fork(1, NULL))
	{
		if (!c4_fork(14, NULL)) {
		} else {
			c4_actv(14, NULL);//
			my_client();
			c4_halt();
		}
	}
	else
	{
		c4_actv(1, NULL);//
		extern void keyboard_server(void);
		keyboard_server();
	}
}
