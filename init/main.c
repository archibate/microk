#include <libl4/ipcmsgs.h>
#include <libl4/l4/api.h>
#include <libl4/rwipc.h>
#include <types.h>
#include <stddef.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>

#define IDLELO   0
#define PATHSVR  1
#define KBDSVR   2
#define COMSVR   3
#define XTMSVR   4
#define CLIENT   5
#define KMEM_CAP 8
#define VRAM_CAP 9

int getich(int svr)
{
	ICH_MSG msg;
	l4_send(svr, NULL);
	l4_recv(svr, &msg);
	return msg.ich;
}

int getichmsg(int svr, ICH_MSG *msg)
{
	l4_send(svr, NULL);
	l4_recv(svr, msg);
	return msg->ich;
}

void txwrite(int svr, const char *buf, size_t size)
{
	TX_MSG msg;
	const size_t FRAGSIZE = sizeof(msg.tx_data);
	for (int i = 0; i < size / FRAGSIZE; i++) {
		memcpy(msg.tx_data, buf, msg.tx_len = FRAGSIZE);
		l4_send(svr, &msg);
		l4_recv(svr, NULL);
	}
	memcpy(msg.tx_data, buf, msg.tx_len = size % FRAGSIZE);
	l4_send(svr, &msg);
	l4_recv(svr, NULL);
}

void txputc(int svr, uchar ch)
{
	TX_MSG msg;
	msg.tx_len = 1;
	msg.tx_data[0] = ch;
	l4_send(svr, &msg);
	l4_recv(svr, NULL);
}

void my_client(void)
{
	ICH_MSG msg;
	int i = open("hello.txt", 0);
	char buf[256];
	ssize_t size = read(i, buf, sizeof(buf));
	l4_puts(buf);
	close(i);
	l4_write(XTMSVR, buf, size);
	l4_write(COMSVR, buf, size);

	while (1) {
		getichmsg(KBDSVR, &msg);
		l4_write(XTMSVR, &msg.ic_raw, strlen(msg.ic_raw));
		l4_write(COMSVR, &msg.ic_raw, strlen(msg.ic_raw));
	}
}

void main(void)
{
	if (!l4_fork(PATHSVR)) {
		if (!l4_fork(KBDSVR)) {
			if (!l4_fork(COMSVR)) {
				if (!l4_fork(XTMSVR)) {
					l4_cmap(VRAM_CAP, 0x0,    0xe0000000,     -1L);
					l4_cmap(KMEM_CAP, 0x7000, 0xdffff000,  0x1000);
					extern int xterm_server(void *vram, void *vinfo_p);
					xterm_server((void*)0xe0000000, (void*)0xdffffb00);
				} else {
					l4_actv(XTMSVR);//
					extern void serial_server(void);
					serial_server();
				}
			} else {
				l4_actv(COMSVR);//
				extern void keyboard_server(void);
				keyboard_server();
			}
		} else {
			l4_actv(KBDSVR);//
			extern void path_server(void);
			path_server();
		}
	} else {
		l4_actv(PATHSVR);//
		my_client();
	}
}
