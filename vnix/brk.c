#include "unix.h"
#include <stddef.h>
#include <memlay.h>

static ulong curr = STACK_BEG;

int brk(void *addr)
{
	ulong adr = (ulong)addr;

	// ...

	curr = adr;
	return 0;
}

void *sbrk(soff_t increment)
{
	void *old = (void *)curr;
	int ret = brk((void*)(curr + increment));
	if (ret < 0)
		return NULL;
	return old;
}
