#include <stdlib.h>
#include <debug.h>
#include <assert.h>

int __attribute__((noreturn)) __assertion_failure(const char *expr)
{
	dbg_printf("assert(%s) failed\n", expr);
	abort();
}
