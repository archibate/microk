#include "unix.h"

extern void _start(void);

int exec(const char *path) // TODO!!!!!
{
	int i = open(path, O_RDONLY | O_EXEC);
	if (i < 0)
		return i;
	ssize_t size = read(i, _start, -1L);
	close(i);
	if (size < 0)
		return size;
	_start();
	__builtin_unreachable();
}
