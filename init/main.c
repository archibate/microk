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
#define C4_NIL  63
#define C4_SYS(to, fr) ((to) | ((fr) << 8))

void main(void)
{
	char *vram    = (char*)0xe0000000;
	//char *lowmem  = (char*)0xa0000000;
	//char *ramdisk = (char*)0xa0007c00;

	for (int i = 0; i < 800 * 600; i++) {
		vram[i] = i % 64;
	}

	UT_REGS rd;
	if (!c4_sysux(C4_FORK, C4_SYS(1, 0), NULL, &rd)) {
		// child here!!
		while (rd.dx == 12);
		*(int*)0xdeadc0de = 0xfacedeaf;
	} else {
		UT_REGS wr;
		wr.dx = 12;
		c4_sysux(C4_IPC, C4_SYS(1, C4_NIL), &wr, NULL);
	}
}
