#include "ptregs.h"
#include "panic.h"
#include "print.h"
#include "ptregs.h"

void dbg_backtrace(void)
{
	void **bp;
	asm volatile ("mov %%ebp, %0" : "=r" (bp));

	printf("stack backtrace:\n");
	for (int i = 0; i < 10 && bp; i++) {
		printf("\t#%d %p\n", i, bp[1]);
		bp = bp[0];
	}
}

void dbg_dumpifregs(IF_REGS *regs)
{
	unsigned long cr2, cr3;
	printf("cpu dump:\n");
	printf("ss:esp=%04X:%p\n", regs->ss, regs->sp);
	printf("cs:eip=%04X:%p\n", regs->cs, regs->pc);
	printf("err=%08X\tefl=%08X\n", regs->x86_errcd, regs->eflags);
	printf("eax=%p\tebx=%p\n", regs->ax, regs->msg0dat.bx);
	printf("ecx=%p\tedx=%p\n", regs->cx, regs->msg0dat.dx);
	printf("esi=%p\tedi=%p\n", regs->msg0dat.si, regs->msg0dat.di);
	printf("ebp=%p\tesp=%p\n", regs->msg0dat.bp, regs->sp);
	asm volatile ("movl %%cr2, %%eax\n" : "=a" (cr2));
	asm volatile ("movl %%cr3, %%eax\n" : "=a" (cr3));
	printf("cr2=%p\tcr3=%p\n", cr2, cr3);
}
