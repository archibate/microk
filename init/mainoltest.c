// vim: fdm=marker
#include <types.h>
#include <stddef.h>
#include <memory.h>
#include <struct.h>
#include <sysl4.h>

#define CLIENT 1
#define SERVER 1
#define TRADER 2
#define KMEM_CAP 3
#define VRAM_CAP 4

void server(char *vram)
{
	UT_REGS reg;
	reg.dx = 0x8a088b45; reg.si = 0x83088b45; reg.di = 0x0f088b45; reg.bx = 0x01088b45; reg.bp = 0x88088b45;
	for (int i = 0; i < 800 * 600; i += sizeof(reg))
	{
		/*if (i % 8800 >= 8800 - sizeof(reg)) */
		l4_recv(CLIENT, &reg);
		memcpy(vram + i, &reg, sizeof(reg));
		l4_send(CLIENT, NULL);
	}
	//*(int*)0xdeadc0de = 0xcafebabe;
}

/*void client(void) // {{{ {
	l4_ipcw(SERVER, 64);
	l4_ipcw(SERVER, 64);
	l4_ipcw(SERVER, 64);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
#if 0
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 64);
	l4_ipcw(SERVER, 64);
	l4_ipcw(SERVER, 64);
	l4_ipcw(SERVER, 64);
	l4_ipcw(SERVER, 64);
	l4_ipcw(SERVER, 64);
	l4_ipcw(SERVER, 64);
	l4_ipcw(SERVER, 64);
	l4_ipcw(SERVER, 64);
	l4_ipcw(SERVER, 64);
	l4_ipcw(SERVER, 64);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 32);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 86);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
#endif
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 16);
	l4_ipcw(SERVER, 64);
} */ // }}}

void client(char *ramdisk)
{
	UT_REGS reg;
	for (int i = 0xc0000; i < 0x400000; i += sizeof(reg))
	{
		memcpy(&reg, ramdisk + i, sizeof(reg));
		//memset(&reg, i / 8000, sizeof(reg));
		l4_send(SERVER, &reg);
		l4_recv(SERVER, NULL);
	}
}

void main(void)
{
	UT_REGS reg;
	if (!l4_fork(CLIENT, &reg))
	{ // CLIENT
			l4_real(KMEM_CAP);
			client((char*)0xa0000000);
	}
	else
	{ // SERVER
		/*if (!l4_fork(TRADER, &reg))
		{ // TRADER
			if (reg.dx != 12)
				*(int*)0xdeadc0de = 0xcafebabe;
			l4_call(TRADER, &reg, NULL);
		}
		else
		{ // SERVER*/
			//reg.dx = 12;
			l4_actv(CLIENT, &reg);
			l4_real(VRAM_CAP);
			server((char*)0xe0000000);
		//}
	}
}
