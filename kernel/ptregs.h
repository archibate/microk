#pragma once

#include <types.h>
#include <struct.h>
#include "l4/defs.h"
#include "caps.h"
#include "msg.h"

// syscall args: ax(cx, dx, bx, di, si, bp)

STRUCT(IF_REGS)
{
	MSGDAT msg0dat;
	ulong cx, ax;
	ulong es, ds, unused1, unused2;
	ulong pc, cs, eflags, sp, ss;
	MSG msgs[MAXMSG];
	CAP C[MAXCAP]; // sized 3072 bytes // try to make this into one page?
	cap_t waitcap;
} __attribute__((packed));

static inline void __unused_func2(void)
{
	switch (0) {
	case sizeof(IF_REGS) <= 4096:
	case 0: break;
	};
}
