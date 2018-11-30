// vim: fdm=marker
#include <types.h>
#include <stddef.h>
#include <memory.h>
#include <struct.h>
#include <sysc4.h>

#define CLIENT 0
#define SERVER 1
#define TRADER 2
#define KMEM_CAP 1
#define VRAM_CAP 2

void server(char *vram)
{
	while (1)
	{
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
#if 0
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
#endif
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
}

void main(void)
{
	UT_REGS reg;
	if (!c4_fork(SERVER, &reg))
	{ // SERVER
		c4_real(VRAM_CAP);
		server((char*)0xe0000000);
	}
	else
	{ // CLIENT
		c4_share(SERVER, VRAM_CAP);
		/*if (!c4_fork(TRADER, &reg))
		{ // TRADER
			if (reg.dx != 12)
				*(int*)0xdeadc0de = 0xcafebabe;
		}
		else
		{ // CLIENT
			c4_actvw(TRADER, 12);*/
			client();
		//}
	}
}
