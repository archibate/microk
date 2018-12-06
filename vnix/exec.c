#include "unix.h"
#include <elf/elf.h>
#include <libl4/l4/api.h>

extern void _userbeg();

int exec(const char *path) // TODO!!!!!
{
	int i = open(path, O_RDONLY | O_EXEC);
	if (i < 0)
		return i;

	ssize_t size = read(i, _userbeg, -1L);

	close(i);
	l4_puts("_userbeg()\n");
	asm volatile ("mov %0, %%esp\npush %1\nret\n" :: "r" (0xfeed1ffc), "r" (_userbeg));
	__builtin_unreachable();
}
