// vim: fdm=marker
//#define c4_halt()                               c4_sysux(0x00009, 0, NULL, NULL) // {{{
/*#define c4_send(to, reg)                       c4_sysux(C4_SEND, to, reg, NULL)
//#define c4_recv(to, reg)                       c4_sysux(C4_RECV, to, NULL, reg)
//#define c4_actv(to, reg)                        c4_sysux(C4_ACTV, to, reg, NULL)
//#define c4_fork(to, reg)                        c4_sysux(C4_FORK, to, NULL, reg)
//#define c4_real(capid)                          c4_sysux(C4_REAL, MKWORD(0, capid), NULL, NULL)
//#define c4_ipcx(type, capid, hisid, to, sd, rv) c4_sysux(MKWORD(type, hisid), MKWORD(to, capid), sd, rv)

*static inline
void c4_actvw(uint to, uint w)
{
	UT_REGS reg;
	reg.dx = w;
	c4_actv(to, &reg);
}

*static inline
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

/*static inline
uint c4_ipcxw(uint type, uchar capid, uchar hisid, uchar to, uint w)
{
	UT_REGS reg;
	reg.dx = w;
	c4_ipcx(type, capid, hisid, to, &reg, &reg);
	return reg.dx;
}
//#define c4_ipcw(to, w)         c4_ipcxw(C4_IPC, 0, 0, to, w)
//#define c4_ipc(to, sd, rv)     c4_ipcx (C4_IPC, 0, 0, to, sd, rv)
//#define c4_share(to, capid, hisid)*/ // }}}
#pragma once

#include "defs.h"
#include <sizet.h>
#include <types.h>
#include <stddef.h>
#include <struct.h>
STRUCT(UT_REGS) // short message, exactly 20 bytes
{
	ulong bx, si, di, dx, bp;
};

#define MKWORD(l, h)  (((l)&0xff)|(((h)&0xff)<<8))
extern uint c4_sysux(uint ax, uint cx, const void *snd, void *rcv);
#define dbg_c4_sysux(ax,...) (c4_sysux(ax,__VA_ARGS__) ? *(volatile int*)(0xdeafbab0 +ax) : 0)
#define c4_puts(s)  c4_sysux(0x17, (uint)(s), NULL, NULL)
#define c4_print(x) c4_sysux(0x18, (x), NULL, NULL)
#define c4_halt()   c4_sysux(0x19, 0, NULL, NULL)

static uint c4_call(uint to, const void *arg, void *ret)
{
	return dbg_c4_sysux(MKWORD(C4_CALL, 0), MKWORD(to, 0), arg, ret);
}

static uint c4_sendmsg(uint to, const void *msg, size_t len)
{
	UT_REGS reg;
	reg.dx = (ulong)msg;
	return dbg_c4_sysux(MKWORD(C4_SEND, len), MKWORD(to, 0), &reg, NULL);
}

static uint c4_waitmsg(uint fr, void *msg, size_t len)
{
	UT_REGS reg;
	reg.dx = (ulong)msg;
	return dbg_c4_sysux(MKWORD(C4_WAIT, len), MKWORD(fr, 0), &reg, NULL);
}

static uint c4_send(uint to, const void *arg)
{
	return dbg_c4_sysux(MKWORD(C4_SEND, 0), MKWORD(to, 0), arg, NULL);
}

static uint c4_wait(uint fr, void *arg)
{
	return dbg_c4_sysux(MKWORD(C4_WAIT, 0), MKWORD(fr, 0), NULL, arg);
}

static uint c4_retn(uint to, const void *ret)
{
	return dbg_c4_sysux(MKWORD(C4_RETN, 0), MKWORD(to, 0), ret, NULL);
}

static uint c4_fork(uint to, void *rcv)
{
	return c4_sysux(MKWORD(C4_FORK, 0), MKWORD(to, 0), NULL, rcv);
}

static uint c4_actv(uint to, const void *arg)
{
	return dbg_c4_sysux(MKWORD(C4_ACTV, 0), MKWORD(to, 0), arg, NULL);
}

static uint c4_real(uint capid)
{
	return dbg_c4_sysux(MKWORD(C4_REAL, 0), MKWORD(0, capid), NULL, NULL);
}
