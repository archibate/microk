// vim: fdm=marker
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
extern int c4_sysux(int ax, int cx, const void *snd, void *rcv);
static int dbg_c4_sysux(int ax, int cx, const void *snd, void *rcv)
{
	int ret = c4_sysux(ax, cx, snd, rcv);
	if (ret < 0) {
		*(volatile int*)(0xdead0000 + (ax << 12) - ret) = 0xdeadbeaf;
	}
	return ret;
}
#define c4_puts(s)  c4_sysux(0x17, (int)(s), NULL, NULL)
#define c4_print(x) c4_sysux(0x18, (x), NULL, NULL)
#define c4_halt()   c4_sysux(0x19, 0, NULL, NULL)

static int c4_send(c4id_t to, const void *arg)
{
	return dbg_c4_sysux(MKWORD(C4_SEND, 0), MKWORD(to, 0), arg, NULL);
}

static c4id_t c4_wait(c4id_t fr, void *arg)
{
	return dbg_c4_sysux(MKWORD(C4_WAIT, 0), MKWORD(fr, 0), NULL, arg);
}

static c4id_t c4_fork(cap_t tocap, c4id_t totid, void *rcv)
{
	return c4_sysux(MKWORD(C4_FORK, 0), MKWORD(tocap, totid), NULL, rcv);
}

static int c4_actv(cap_t to, const void *arg)
{
	return dbg_c4_sysux(MKWORD(C4_ACTV, 0), MKWORD(to, 0), arg, NULL);
}

static int c4_real(cap_t capid)
{
	return dbg_c4_sysux(MKWORD(C4_REAL, 0), MKWORD(0, capid), NULL, NULL);
}
#if 0 // unimpl. api {{{
static int c4_call(int to, const void *arg, void *ret)
{
	return dbg_c4_sysux(MKWORD(C4_CALL, 0), MKWORD(to, 0), arg, ret);
}

static int c4_sendmsg(int to, const void *msg, size_t len)
{
	UT_REGS reg;
	reg.dx = (ulong)msg;
	return dbg_c4_sysux(MKWORD(C4_SEND, len), MKWORD(to, 0), &reg, NULL);
}

static int c4_waitmsg(int fr, void *msg, size_t len)
{
	UT_REGS reg;
	reg.dx = (ulong)msg;
	return dbg_c4_sysux(MKWORD(C4_WAIT, len), MKWORD(fr, 0), &reg, NULL);
}

static int c4_retn(int to, const void *ret)
{
	return dbg_c4_sysux(MKWORD(C4_RETN, 0), MKWORD(to, 0), ret, NULL);
}
#endif // }}}
//#define c4_halt()                               c4_sysux(0x00009, 0, NULL, NULL) // {{{
/*#define c4_send(to, reg)                       c4_sysux(C4_SEND, to, reg, NULL)
//#define c4_recv(to, reg)                       c4_sysux(C4_RECV, to, NULL, reg)
//#define c4_actv(to, reg)                        c4_sysux(C4_ACTV, to, reg, NULL)
//#define c4_fork(to, reg)                        c4_sysux(C4_FORK, to, NULL, reg)
//#define c4_real(capid)                          c4_sysux(C4_REAL, MKWORD(0, capid), NULL, NULL)
//#define c4_ipcx(type, capid, hisid, to, sd, rv) c4_sysux(MKWORD(type, hisid), MKWORD(to, capid), sd, rv)

*static inline
void c4_actvw(int to, int w)
{
	UT_REGS reg;
	reg.dx = w;
	c4_actv(to, &reg);
}

*static inline
void c4_sendw(int to, int w)
{
	UT_REGS reg;
	reg.dx = w;
	c4_send(to, &reg);
}

static inline
int c4_recvw(int to)
{
	UT_REGS reg;
	c4_recv(to, &reg);
	return reg.dx;
}*/

/*static inline
int c4_ipcxw(int type, uchar capid, uchar hisid, uchar to, int w)
{
	UT_REGS reg;
	reg.dx = w;
	c4_ipcx(type, capid, hisid, to, &reg, &reg);
	return reg.dx;
}
//#define c4_ipcw(to, w)         c4_ipcxw(C4_IPC, 0, 0, to, w)
//#define c4_ipc(to, sd, rv)     c4_ipcx (C4_IPC, 0, 0, to, sd, rv)
//#define c4_share(to, capid, hisid)*/ // }}}
