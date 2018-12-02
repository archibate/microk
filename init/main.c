// vim: fdm=marker
#include <libl4/ipcmsgs.h>
#include <libl4/l4/api.h>
#include <types.h>
#include <stddef.h>
#include <memory.h>
#include <string.h>

#define CLIENT   0
#define KBDSVR   1
#define COMSVR   2
#define XTMSVR   3
#define IDLELO   4
#define KMEM_CAP 8
#define VRAM_CAP 9

int getich(int svr)
{
	ICH_MSG msg;
	l4_send(svr, NULL);
	l4_wait(svr, &msg);
	return msg.ich;
}

int getichmsg(int svr, ICH_MSG *msg)
{
	l4_send(svr, NULL);
	l4_wait(svr, msg);
	return msg->ich;
}

void txwrite(int svr, const char *buf, size_t size)
{
	TX_MSG msg;
	const size_t FRAGSIZE = sizeof(msg.tx_data);
	for (int i = 0; i < size / FRAGSIZE; i++) {
		memcpy(msg.tx_data, buf, msg.tx_len = FRAGSIZE);
		l4_send(svr, &msg);
		l4_wait(svr, NULL);
	}
	memcpy(msg.tx_data, buf, msg.tx_len = size % FRAGSIZE);
	l4_send(svr, &msg);
	l4_wait(svr, NULL);
}

void txputc(int svr, uchar ch)
{
	TX_MSG msg;
	msg.tx_len = sizeof(ch);
	msg.tx_data[0] = ch;
	l4_send(svr, &msg);
	l4_wait(svr, NULL);
}

void my_client(void)
{
	ICH_MSG msg;
	while (1) {
		getichmsg(KBDSVR, &msg);
		l4_send(XTMSVR, &msg);
		//txwrite(COMSVR, msg.ic_raw, strlen(msg.ic_raw));
		l4_wait(XTMSVR, NULL);
	}
}

void main(void)
{
	if (!l4_fork(KBDSVR, 1, NULL))
	{
		if (!l4_fork(COMSVR, 2, NULL)) {
			if (!l4_fork(XTMSVR, 3, NULL)) {
				if (!l4_fork(IDLELO, 4, NULL)) {
					l4_halt();
				} else {
					l4_actv(4, NULL);//
					l4_real(VRAM_CAP);
					l4_real(KMEM_CAP);
					extern int xterm_server(void *vram, void *vinfo_p);
					xterm_server((void*)0xe0000000, (void*)0xa0007b00);
				}
			} else {
				l4_actv(3, NULL);//
				extern void serial_server(void);
				serial_server();
			}
		} else {
			l4_actv(2, NULL);//
			extern void keyboard_server(void);
			keyboard_server();
		}
	}
	else
	{
		l4_actv(1, NULL);//
		my_client();
	}
}
