#include <types.h>
#include <memory.h>

void main(void)
{
	char *vram = (char*)0xfd000000;

	for (int i = 0; i < 800 * 600; i++) {
		vram[i] = i % 64;
	}
}
