// vim: fdm=marker
#pragma once

#include <krnl4/l4/defs.h>
#include <sizet.h>
#include <types.h>
#include <stddef.h>
#include <struct.h>
STRUCT(UT_REGS) // short message, exactly 20 bytes
{
	ulong bx, si, di, dx, bp;
};

#define MKWORD(l, h)  (((l)&0xff)|(((h)&0xff)<<8))
extern int l4_sysux(int ax, int cx, const void *snd, void *rcv);
static int dbg_l4_sysux(int ax, int cx, const void *snd, void *rcv)
{
	int ret = l4_sysux(ax, cx, snd, rcv);
	if (ret < 0) {
		*(volatile int*)(0xdead0000 + (ax << 12) - ret) = 0xdeadbeaf;
	}
	return ret;
}
#define l4_puts(s)  l4_sysux(0x17, (int)(s), NULL, NULL)
#define l4_print(x) l4_sysux(0x18, (x), NULL, NULL)
#define l4_halt()   l4_sysux(0x19, 0, NULL, NULL)

static int l4_send(l4id_t to, const void *arg)
{
	return dbg_l4_sysux(MKWORD(L4_SEND, 0), MKWORD(to, 0), arg, NULL);
}

static l4id_t l4_wait(l4id_t fr, void *arg)
{
	return dbg_l4_sysux(MKWORD(L4_WAIT, 0), MKWORD(fr, 0), NULL, arg);
}

static l4id_t l4_fork(cap_t tocap, l4id_t totid, void *rcv)
{
	return l4_sysux(MKWORD(L4_FORK, 0), MKWORD(tocap, totid), NULL, rcv);
}

static int l4_actv(cap_t to, const void *arg)
{
	return dbg_l4_sysux(MKWORD(L4_ACTV, 0), MKWORD(to, 0), arg, NULL);
}

static int l4_real(cap_t capid)
{
	return dbg_l4_sysux(MKWORD(L4_REAL, 0), MKWORD(0, capid), NULL, NULL);
}
#if 0 // unimpl. api {{{
static int l4_call(int to, const void *arg, void *ret)
{
	return dbg_l4_sysux(MKWORD(L4_CALL, 0), MKWORD(to, 0), arg, ret);
}

static int l4_sendmsg(int to, const void *msg, size_t len)
{
	UT_REGS reg;
	reg.dx = (ulong)msg;
	return dbg_l4_sysux(MKWORD(L4_SEND, len), MKWORD(to, 0), &reg, NULL);
}

static int l4_waitmsg(int fr, void *msg, size_t len)
{
	UT_REGS reg;
	reg.dx = (ulong)msg;
	return dbg_l4_sysux(MKWORD(L4_WAIT, len), MKWORD(fr, 0), &reg, NULL);
}

static int l4_retn(int to, const void *ret)
{
	return dbg_l4_sysux(MKWORD(L4_RETN, 0), MKWORD(to, 0), ret, NULL);
}
#endif // }}}
//#define l4_halt()                               l4_sysux(0x00009, 0, NULL, NULL) // {{{
/*#define l4_send(to, reg)                       l4_sysux(L4_SEND, to, reg, NULL)
//#define l4_recv(to, reg)                       l4_sysux(L4_RECV, to, NULL, reg)
//#define l4_actv(to, reg)                        l4_sysux(L4_ACTV, to, reg, NULL)
//#define l4_fork(to, reg)                        l4_sysux(L4_FORK, to, NULL, reg)
//#define l4_real(capid)                          l4_sysux(L4_REAL, MKWORD(0, capid), NULL, NULL)
//#define l4_ipcx(type, capid, hisid, to, sd, rv) l4_sysux(MKWORD(type, hisid), MKWORD(to, capid), sd, rv)

*static inline
void l4_actvw(int to, int w)
{
	UT_REGS reg;
	reg.dx = w;
	l4_actv(to, &reg);
}

*static inline
void l4_sendw(int to, int w)
{
	UT_REGS reg;
	reg.dx = w;
	l4_send(to, &reg);
}

static inline
int l4_recvw(int to)
{
	UT_REGS reg;
	l4_recv(to, &reg);
	return reg.dx;
}*/

/*static inline
int l4_ipcxw(int type, uchar capid, uchar hisid, uchar to, int w)
{
	UT_REGS reg;
	reg.dx = w;
	l4_ipcx(type, capid, hisid, to, &reg, &reg);
	return reg.dx;
}
//#define l4_ipcw(to, w)         l4_ipcxw(L4_IPC, 0, 0, to, w)
//#define l4_ipc(to, sd, rv)     l4_ipcx (L4_IPC, 0, 0, to, sd, rv)
//#define l4_share(to, capid, hisid)*/ // }}}
