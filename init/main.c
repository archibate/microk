#include <types.h>
#include <memory.h>

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
	} else {
		c4_halt();
	}
	*(int*)0xdeadc0de = 0xfacedeaf;
}
