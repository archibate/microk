#include "panic.h"
#include "print.h"
#include <stdarg.h>
#include <memory.h>
#define DDDUMP
#ifdef DDDUMP
#include <debug.h>
#include "ptregs.h"
#endif

static
void __attribute__((noreturn)) cpu_halt(void)
{
	asm volatile ("cli\nhlt\n");
	__builtin_unreachable();
}


static
void __attribute__((noreturn)) halt(void)
{
#ifdef DDDUMP
	dbg_dumpifregs((IF_REGS*)0x400000);
#endif

	memset((char *) 0xa0000, 0x0c, 320 * 1);

	cpu_halt();
}


void __attribute__((noreturn)) panic
		( const char *fmt
		, ...
		)
{
	va_list ap;
	va_start(ap, fmt);
	printf(C_EMPH C_RED "PANIC: ");
	vprintf(fmt, ap);
	printf(C_NORM "\n");
	va_end(ap);

	halt();
}
