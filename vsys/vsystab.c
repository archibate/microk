#include "unix.h"

void *__attribute__((section(".vsystab"))) vsystab[] = {
	open, read, write, close, exec,
};

void _start(void)
{
	for (;;); // TODO!
}
