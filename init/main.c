// vim: fdm=marker
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
#define C4_RELA 1
#define C4_FORK 2
#define C4_ANY  62
#define C4_NIL  63
#define C4_SYS(to, fr) ((to) | ((fr) << 8))

#define c4_fork(to, reg)   c4_sysux(C4_FORK, to, NULL, reg)
#define c4_rela(to, reg)   c4_sysux(C4_RELA, to, reg, NULL)
#define c4_ipc(to, sd, rv) c4_sysux(C4_IPC,  to,  sd , rv )

void c4_relaw(uint to, uint w)
{
	UT_REGS reg;
	reg.dx = w;
	c4_rela(to, &reg);
}

uint c4_ipcw(uint to, uint w)
{
	UT_REGS reg;
	reg.dx = w;
	c4_ipc(to, &reg, &reg);
	return reg.dx;
}

#define SERVER 0
#define CLIENT 1
#define TRADER 2

void server(void)
{
	char *vram = (char*)0xe0000000;
	while (1)
	{
		//uint max = c4_rplyrecvch(CLIENT, CLIENT, 12);
		uint max = c4_ipcw(CLIENT, 12);
		for (int i = 0; i < 800 * 600; i++)
			vram[i] = i % max;
		//*(int*)0xdeadc0de = 0xcafebabe;
	}
}

void client(void) // {{{
{
	c4_ipcw(SERVER, 64);
	c4_ipcw(SERVER, 64);
	c4_ipcw(SERVER, 64);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 64);
	c4_ipcw(SERVER, 64);
	c4_ipcw(SERVER, 64);
	c4_ipcw(SERVER, 64);
	c4_ipcw(SERVER, 64);
	c4_ipcw(SERVER, 64);
	c4_ipcw(SERVER, 64);
	c4_ipcw(SERVER, 64);
	c4_ipcw(SERVER, 64);
	c4_ipcw(SERVER, 64);
	c4_ipcw(SERVER, 64);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 32);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 86);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 16);
	c4_ipcw(SERVER, 64);
} // }}}

void trader(void)
{
	*(int*)0xdeadc0de = 0xcafebabe;
}

void main(void)
{
	UT_REGS reg;
	if (!c4_fork(CLIENT, &reg))
	{ // CLIENT
		if (!c4_fork(TRADER, &reg))
		{ // TRADER
			trader();
		}
		else
		{ // CLIENT
			c4_relaw(TRADER, 12);
			client();
		}
	}
	else
	{ // SERVER
		server();
	}
}
