#pragma once

#include <struct.h>
STRUCT(UT_REGS) // short message, exactly 20 bytes
{
	ulong bx, si, di, dx, bp;
};

extern uint c4_sysux(uint ax, uint cx, const void *wr, const void *rd);

#define C4_IPC   0
#define C4_ACTV  1
#define C4_FORK  2
#define C4_SHARE 3
#define C4_GRANT 4
#define C4_REAL  5
#define C4_ANY   255
#define C4_NIL   255

//#define c4_send(to, reg)   c4_sysux(C4_SEND, to, reg, NULL)
//#define c4_recv(to, reg)   c4_sysux(C4_RECV, to, NULL, reg)
#define c4_actv(to, reg)   c4_sysux(C4_ACTV, to, reg, NULL)
#define c4_fork(to, reg)   c4_sysux(C4_FORK, to, NULL, reg)
#define c4_ipc(to, sd, rv) c4_sysux(C4_IPC,  to,  sd , rv )

static inline
uint c4_syscall(uint ax, uint cx, uint dx)
{
	uint res;
	asm volatile ("int $0x80" : "=a" (res) : "a" (ax), "c" (cx), "d" (dx) :);
	return res;
}

#define c4_share(to, capid) c4_syscall(C4_SHARE, to, capid)
#define c4_grant(to, capid) c4_syscall(C4_GRANT, to, capid)
#define c4_real(     capid) c4_syscall(C4_REAL,   0, capid)

static inline
void c4_actvw(uint to, uint w)
{
	UT_REGS reg;
	reg.dx = w;
	c4_actv(to, &reg);
}

/*static inline
void c4_sendw(uint to, uint w)
{
	UT_REGS reg;
	reg.dx = w;
	c4_send(to, &reg);
}

static inline
uint c4_recvw(uint to)
{
	UT_REGS reg;
	c4_recv(to, &reg);
	return reg.dx;
}*/

static inline
uint c4_ipcw(uint to, uint w)
{
	UT_REGS reg;
	reg.dx = w;
	c4_ipc(to, &reg, &reg);
	return reg.dx;
}
#define c4_sendw(to, w) ((void)c4_ipcw(to, w))
#define c4_recvw(to)    (c4_ipcw(to, 12))
