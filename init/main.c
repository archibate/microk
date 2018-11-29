#include <types.h>
#include <stddef.h>
#include <memory.h>
#include <struct.h>

STRUCT(UT_REGS)
{
	ulong bx, si, di, dx, bp;
};

extern uint c4_sysux(uint ax, uint cx, const UT_REGS *wr, const UT_REGS *rd);

uint syscall(uint ax, uint cx, uint dx)
{
	uint res;
	asm volatile ("int $0x80" : "=a" (res) : "a" (ax), "c" (cx), "d" (dx) :);
	return res;
}

#define C4_IPC  0
#define C4_FORK 2
#define C4_ANY  62
#define C4_NIL  63
#define C4_SYS(to, fr) ((to) | ((fr) << 8))

#define c4_fork(to, reg)  c4_sysux(C4_FORK, C4_SYS(to, C4_NIL), NULL, reg)
#define c4_send(to, reg)  c4_sysux(C4_IPC,  C4_SYS(to, C4_NIL), reg, NULL)
#define c4_recv(fr, reg)  c4_sysux(C4_IPC,  C4_SYS(C4_NIL, fr), NULL, reg)
/*#define c4_sendrecv(to, fr, regsd, regrv) \
	                  c4_sysux(C4_IPC,  C4_SYS(to, fr), regsd, regrv)*/

void c4_sendch(uint to, uint ch)
{
	UT_REGS reg;
	reg.dx = ch;
	c4_send(to, &reg);
}

uint c4_recvch(uint fr)
{
	UT_REGS reg;
	c4_recv(fr, &reg);
	return reg.dx;
}

#define CLIENT 1
#define SERVER 0

void server(void)
{
	char *vram = (char*)0xe0000000;
	while (1)
	{
		uint max = c4_recvch(CLIENT);
		/*for (int i = 0; i < 800 * 600; i++)
			vram[i] = i % max;*/
		//*(int*)0xdeadc0de = 0xcafebabe;
	}
}

void client(void)
{
	c4_sendch(SERVER, 64);
	c4_sendch(SERVER, 64);
	c4_sendch(SERVER, 64);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 64);
	c4_sendch(SERVER, 64);
	c4_sendch(SERVER, 64);
	c4_sendch(SERVER, 64);
	c4_sendch(SERVER, 64);
	c4_sendch(SERVER, 64);
	c4_sendch(SERVER, 64);
	c4_sendch(SERVER, 64);
	c4_sendch(SERVER, 64);
	c4_sendch(SERVER, 64);
	c4_sendch(SERVER, 64);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 32);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 86);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 16);
	c4_sendch(SERVER, 64);
}

void main(void)
{
	UT_REGS reg;
	if (!c4_fork(CLIENT, &reg))
	{ // 1
		client();
	}
	else
	{ // 0
		c4_sendch(CLIENT, 12); // activate the child process
		server();
	}
}
