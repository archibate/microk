#pragma once

#include <struct.h>
STRUCT(UT_REGS) // short message, exactly 20 bytes
{
	ulong bx, si, di, dx, bp;
};

extern uint c4_sysux(uint ax, uint cx, const void *wr, const void *rd);

#define C4_IPC  0
#define C4_ACTV 1
#define C4_FORK 2
#define C4_SMAP 3
#define C4_GRNT 4
#define C4_REAL 5
#define C4_ANY  62
#define C4_NIL  63

#define c4_fork(to, reg)   c4_sysux(C4_FORK, to, NULL, reg)
#define c4_actv(to, reg)   c4_sysux(C4_ACTV, to, reg, NULL)
#define c4_ipc(to, sd, rv) c4_sysux(C4_IPC,  to,  sd , rv )

static inline
uint c4_syscall(uint ax, uint cx, uint dx, uint si)
{
	uint res;
	asm volatile ("int $0x80" : "=a" (res) : "a" (ax), "c" (cx), "d" (dx), "S" (si) :);
	return res;
}

#define c4_smap(to, va, size) c4_syscall(C4_SMAP, to, va, size)
#define c4_grnt(to, va, size) c4_syscall(C4_GRNT, to, va, size)
#define c4_real(    va, size) c4_syscall(C4_REAL,  0, va, size)

static inline
void c4_actvw(uint to, uint w)
{
	UT_REGS reg;
	reg.dx = w;
	c4_actv(to, &reg);
}

static inline
uint c4_ipcw(uint to, uint w)
{
	UT_REGS reg;
	reg.dx = w;
	c4_ipc(to, &reg, &reg);
	return reg.dx;
}