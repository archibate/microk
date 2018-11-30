#pragma once

#include <types.h>
#include <struct.h>
#include "caps.h"

// syscall args: ax(cx, dx, bx, di, si, bp)

#define MAXCAP 256
typedef uchar cap_t; // auto warp for MAXCAP = 256

STRUCT(IF_REGS)
{
	char buff[4072];
	union {
		struct { unsigned long di, si, bp, unused_sp, bx, dx, cx, ax; };
		char needcpy[24];
	};
	unsigned long es, ds, unused1, unused2;
	unsigned long pc, cs, eflags, sp, ss;
	CAP C[MAXCAP]; // sized 3072 bytes
} __attribute__((packed));

static inline void __unused_func2(void)
{
	switch (0) {
	case sizeof(IF_REGS) == 4096+44+3072:
	case 0: break;
	};
	switch (0) {
	case sizeof(IF_REGS) <= 4096*2:
	case 0: break;
	};
}
