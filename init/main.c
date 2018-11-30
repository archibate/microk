// vim: fdm=marker
#include <types.h>
#include <stddef.h>
#include <memory.h>
#include <struct.h>
#include <sysc4.h>

#define CLIENT 1
#define SERVER 1
#define TRADER 2
#define KMEM_CAP 3
#define VRAM_CAP 4

void server(char *vram)
{
	UT_REGS reg;
	for (int i = 0; i < 800 * 600; i += sizeof(reg))
	{
		c4_wait(CLIENT, &reg);
		memcpy(vram + i, &reg, sizeof(reg));
		c4_retn(CLIENT, NULL);
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
	for (int i = 0xc0000; i < 0x400000; i += sizeof(reg))
	{
		//reg.dx = 0x8a088b45; reg.si = 0x83088b45; reg.di = 0x0f088b45; reg.bx = 0x01088b45; reg.bp = 0x88088b45;
		memcpy(&reg, ramdisk + i, sizeof(reg));
		//memset(&reg, i / 8000, sizeof(reg));
		c4_call(SERVER, &reg, NULL);
	}
}

void main(void)
{
	UT_REGS reg;
	if (!c4_fork(SERVER, &reg))
	{ // CLIENT
			c4_real(KMEM_CAP);
			client((char*)0xa0000000);
	}
	else
	{ // SERVER
		/*if (!c4_fork(TRADER, &reg))
		{ // TRADER
			if (reg.dx != 12)
				*(int*)0xdeadc0de = 0xcafebabe;
		}
		else
		{ // SERVER
			c4_actvw(TRADER, 12);*/
			c4_actv(CLIENT, &reg);
			c4_real(VRAM_CAP);
			server((char*)0xe0000000);
		//}
	}
}
