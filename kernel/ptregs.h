#pragma once

#include <struct.h>

// syscall args: ax(cx, dx, bx, di, si, bp)

STRUCT(PT_REGS)
{
	unsigned long di, si, bp, unused_sp, bx, dx, cx, ax;
	unsigned long es, ds;
	unsigned long pc, cs, eflags, sp, ss;
} __attribute__((packed));
