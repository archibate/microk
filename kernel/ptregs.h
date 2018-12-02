#pragma once

#include <types.h>
#include <struct.h>
#include "caps.h"
#include "msg.h"

// syscall args: ax(cx, dx, bx, di, si, bp)

#define MAXCAP 256
typedef uchar cap_t; // auto warp for MAXCAP = 256

STRUCT(IF_REGS)
{
	MSGDAT msg0dat;
	ulong cx, ax;
	ulong es, ds, unused1, unused2;
	ulong pc, cs, eflags, sp, ss;
	MSG msgs[MAXMSG];
	CAP C[MAXCAP]; // sized 3072 bytes // try to make this into one page?
} __attribute__((packed));

static inline void __unused_func2(void)
{
	switch (0) {
	case sizeof(IF_REGS) <= 4096:
	case 0: break;
	};
}
