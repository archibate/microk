// vim: fdm=marker
#include <types.h>
#include <stddef.h>
#include <memory.h>
#include <struct.h>
#include <sysc4.h>

#define CLIENT 0
#define SERVER 1
#define TRADER 2
#define KMEM_CAP 3
#define VRAM_CAP 4

void server(char *vram)
{
	UT_REGS reg;
	for (int i = 0; i < 800 * 600; i += sizeof(reg))
	{
		c4_ipc(CLIENT, NULL, &reg);
		memcpy(vram + i, &reg, sizeof(reg));
	}
	*(int*)0xdeadc0de = 0xcafebabe;
}

/*
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
*/
void client(char *ramdisk)
{
	UT_REGS reg;
	for (int i = 0; i < 0x40000; i += sizeof(reg))
	{
		memcpy(&reg, ramdisk + i, sizeof(reg));
		c4_ipc(SERVER, &reg, NULL);
		if (i > 0x3fed4)
			i = 0;
	}
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
		/*if (!c4_fork(TRADER, &reg))
		{ // TRADER
			if (reg.dx != 12)
				*(int*)0xdeadc0de = 0xcafebabe;
		}
		else
		{ // CLIENT
			c4_actvw(TRADER, 12);*/
			c4_real(KMEM_CAP);
			client((char*)0xa0000000);
		//}
	}
}
