#include <types.h>
#include <memory.h>

uint syscall(uint ax, uint cx, uint dx)
{
	uint res;
	asm volatile ("int $0x80" : "=a" (res) : "a" (ax), "c" (cx), "d" (dx) :);
	return res;
}

void main(void)
{
	char *vram    = (char*)0xe0000000;
	//char *lowmem  = (char*)0xa0000000;
	//char *ramdisk = (char*)0xa0007c00;

	for (int i = 0; i < 800 * 600; i++) {
		vram[i] = i % 64;
	}

	if (syscall(2, 1, 0)) {
		syscall(1, 1, 0);
	} else {
		syscall(9, 0, 0);
	}
	*(int*)0xdeadc0de = 0xfacedeaf;
}
