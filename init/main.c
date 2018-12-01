// vim: fdm=marker
#include "ipcmsgs.h"
#include <types.h>
#include <stddef.h>
#include <memory.h>
#include <c4/api.h>

#define CLIENT   0
#define KBDSVR   1
#define COMSVR   2
#define IDLELO   3
#define KMEM_CAP 8
#define VRAM_CAP 9

int getich(int svr)
{
	ICH_MSG msg;
	c4_send(svr, NULL);
	c4_wait(svr, &msg);
	return msg.ich;
}

void txwrite(int svr, const char *buf, size_t size)
{
	TX_MSG msg;
	const size_t FRAGSIZE = sizeof(msg.tx_data);
	for (int i = 0; i < size / FRAGSIZE; i++) {
		memcpy(msg.tx_data, buf, msg.tx_len = FRAGSIZE);
		c4_send(svr, &msg);
		c4_wait(svr, NULL);
	}
	memcpy(msg.tx_data, buf, msg.tx_len = size % FRAGSIZE);
	c4_send(svr, &msg);
	c4_wait(svr, NULL);
}

void my_client(void)
{
	while (1) {
		uint ich = getich(KBDSVR);
		txwrite(COMSVR, (void*)&ich, 1);
	}
}

void main(void)
{
	if (!c4_fork(KBDSVR, 1, NULL))
	{
		if (!c4_fork(COMSVR, 2, NULL)) {
			if (!c4_fork(IDLELO, 3, NULL)) {
			} else {
				c4_actv(3, NULL);//
				extern void serial_server(void);
				serial_server();
			}
		} else {
			c4_actv(2, NULL);//
			extern void keyboard_server(void);
			keyboard_server();
		}
	}
	else
	{
		c4_actv(1, NULL);//
		my_client();
		c4_halt();
	}
}
