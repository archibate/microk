#include <types.h>
#include <stddef.h>
#include <memory.h>
#include <struct.h>

STRUCT(UT_REGS)
{
	ulong bx, si, di, dx, bp;
};

extern void c4_ipcux(uint to, uint fr, const UT_REGS *wr, const UT_REGS *rd);

uint syscall(uint ax, uint cx, uint dx)
{
	uint res;
	asm volatile ("int $0x80" : "=a" (res) : "a" (ax), "c" (cx), "d" (dx) :);
	return res;
}

#define c4_ipc(to, fr) syscall(0, to, fr)
#define c4_swch(to)    syscall(1, to, 0)
#define c4_fork(to)    syscall(2, to, 0)
#define c4_halt()      syscall(9, 0, 0)

void main(void)
{
	char *vram    = (char*)0xe0000000;
	//char *lowmem  = (char*)0xa0000000;
	//char *ramdisk = (char*)0xa0007c00;

	for (int i = 0; i < 800 * 600; i++) {
		vram[i] = i % 64;
	}

	if (c4_fork(1)) {
		c4_swch(1);
		//UT_REGS wr;
		//c4_ipcux(1, 255, &wr, NULL);
	} else {
		//UT_REGS rd;
		//c4_ipcux(255, 0, NULL, &rd);
		c4_halt();
	}
	*(int*)0xdeadc0de = 0xfacedeaf;
}
