#include <stdlib.h>
#include <debug.h>

void __attribute__((noreturn)) abort(void)
{
	dbg_printf("aborted\n");
	exit(1);
}
