#pragma once

#include <struct.h>

// syscall args: ax(cx, dx, bx, di, si, bp)

STRUCT(IF_REGS)
{
	char buff[4072];
	union {
		struct { unsigned long di, si, bp, unused_sp, bx, dx, cx, ax; };
		char needcpy[24];
	};
	unsigned long es, ds, unused1, unused2;
	unsigned long pc, cs, eflags, sp, ss;
} __attribute__((packed));
