#include <l4/ipcmsgs.h>
#include <l4/l4api.h>
#include <l4/capipc.h>
#include <l4/rwipc.h>
#include <fs/proto.h>
#include <inttypes.h>
#include <stddef.h>
#include <memory.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <l4ids.h>
#include <debug.h>

#if 0 // {{{
int getichmsg(int svr, ICH_MSG *msg)
{
	l4_send(svr, 1, NULL);
	l4_recv(svr, 2, msg);
	return msg->ich;
}

void txwrite(int svr, const char *buf, size_t size)
{
	TX_MSG msg;
	const size_t FRAGSIZE = sizeof(msg.tx_data);
	for (int i = 0; i < size / FRAGSIZE; i++) {
		memcpy(msg.tx_data, buf, msg.tx_len = FRAGSIZE);
		l4_send(svr, 1, &msg);
		l4_recv(svr, 2, NULL);
	}
	memcpy(msg.tx_data, buf, msg.tx_len = size % FRAGSIZE);
	l4_send(svr, 1, &msg);
	l4_recv(svr, 2, NULL);
}

void txputc(int svr, uchar ch)
{
	TX_MSG msg;
	msg.tx_len = 1;
	msg.tx_data[0] = ch;
	l4_send(svr, 1, &msg);
	l4_recv(svr, 2, NULL);
}

void my_client(void)
{
	ICH_MSG msg;
	int i = open("hello.txt", 0);
	if (i < 0)
		dbg_halt();
	char buf[256];
	ssize_t size = read(i, buf, sizeof(buf));
#if 1
	//l4_puts(buf);
	close(i);
	fi_write(L4ID_XTMSVR, buf, size);
	l4_write(L4ID_COMSVR, buf, size);
#endif

	while (1) {
		getichmsg(L4ID_KBDSVR, &msg);
		fi_write(L4ID_XTMSVR, &msg.ic_raw, strlen(msg.ic_raw));
		//l4_write(L4ID_XTMSVR, &msg.ic_raw, strlen(msg.ic_raw));
		l4_write(L4ID_COMSVR, &msg.ic_raw, strlen(msg.ic_raw));
	}
}
#endif // }}}

void main(void)
{
	if (!fork()) {
		if (!fork()) {
			if (!fork()) {
				if (!fork()) {
					/*if (!fork()) {
					// {{{
					//l4_cmap(VRAM_CAP, 0x0,    0x80000000,     -1L);
					//l4_cmap(KMEM_CAP, 0x7000, 0x9ffff000,  0x1000);
#if 0
					l4_mkcap(123, 0x80000000);
					l4_sendcap(0, 15, 123); 
					l4_mkcap(123, 0x80001000);
					l4_sendcap(0, 14, 123); 
#if 0
					LWR_CLI lwr;
					l4_lwrpga_accept(&lwr, L4_ANY);
					const void *p = l4_lwrpga_getptr(&lwr);
					l4_puts(p); // Hello, XTERM!
					l4_lwrpga_reply(&lwr, 0);
#else
					char buf[256];
					//l4_puts("main: l4_lwread\n");
					l4_lwread(L4_ANY, buf, sizeof(buf));
					l4_write(COMSVR, &buf, strlen(buf));
#endif
#endif // }}}
						openat(1, "/dev/mon0", O_WRONLY);
						openat(2, "/dev/mon0", O_WRONLY);
						_exit(exec("/sys/ramfs"));
					}*/
					openat(1, "/dev/mon0", O_WRONLY);
					openat(2, "/dev/mon0", O_WRONLY);
					_exit(exec("/sys/xterm"));
				} else {
					extern void serial_server(void);
					serial_server();
				}
			} else {
				extern void keyboard_server(void);
				keyboard_server();
			}
		} else {
			extern void path_server(void);
			path_server();
		}
	} else {
#if 0 // {{{
		l4_recvcap(L4_ANY, 15, 125);
		l4_cmap(125, 0x0, 0x60000000, -1L);
		memset((void*)0x60000000, 0x07, 4096);
		l4_recvcap(L4_ANY, 14, 125);
		l4_cmap(125, 0x0, 0x60000000, -1L);
		memset((void*)0x60000000, 0x08, 4096);
		static char buf[] = "Hello, XTERM!\n";
		l4_lwrite(XTMSVR, buf, sizeof(buf));
		//char s[] =  "Hello, LWRITE!\n";
#endif // }}}
		openat(0, "/dev/kbd0", O_RDONLY);
		openat(1, "/dev/vmon0", O_WRONLY);
		openat(2, "/dev/vmon0", O_WRONLY);
		_exit(exec("/bin/shell"));
	}
}
