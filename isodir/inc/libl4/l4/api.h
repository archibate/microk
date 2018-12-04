// vim: fdm=marker
#pragma once

#include <libl4/l4/defs.h>
#include <types.h>
#include <stddef.h>
#include <struct.h>

#define __L4_MSG void

STRUCT(l4_recv_ret_t)
{
	int ret;
	uint flags;
};

static l4_recv_ret_t __l4_sysux(int ax, int cx, const __L4_MSG *snd, __L4_MSG *rcv)
{
	l4_recv_ret_t r;
	asm volatile (	"call __asm_l4_sysux"
			: "=a" (r.ret), "=c" (r.flags), "=d" (snd), "=b" (rcv)
			: "a" (ax), "c" (cx), "d" (snd), "b" (rcv)
			: "cc", "memory"
		     );
#if 1 // remove this when everything is complete. {{{
	if (r.ret < 0) {
		// well.. emm.. ugly method of debug but useful sometimes..
		*(volatile int*)(0xdead0000 + (ax << 12) - r.ret) = 0xdeadbeaf;
	}
#endif // }}}
	return r;
}
static int l4_sysux(int ax, int cx, const __L4_MSG *snd, __L4_MSG *rcv)
{
	return __l4_sysux(ax, cx, snd, rcv).ret;
}

#define l4_puts(s)  l4_sysux(0x17, (int)(s), NULL, NULL)
#define l4_print(x) l4_sysux(0x18, (x), NULL, NULL)
#define l4_halt()   l4_sysux(0x19, 0, NULL, NULL)
#define MKWORD(l, h)  (((l)&0xff)|(((h)&0xff)<<8))

#define l4_send(to, msg) l4_send_ex(to, msg, L4_BLOCK)
#define l4_send_ex(to, msg, flags) __l4_send(to, msg, flags, 0)
static int __l4_send(l4id_t to, const __L4_MSG *msg, uint flags, cap_t capid)
{
	return l4_sysux(MKWORD(L4_SEND, flags), MKWORD(to, capid), msg, NULL);
}

static l4_recv_ret_t __l4_recv(l4id_t fr, __L4_MSG *msg, cap_t capid)
{
	return __l4_sysux(MKWORD(L4_RECV, 0), MKWORD(fr, capid), NULL, msg);
}

static inline l4id_t l4_recv(l4id_t fr, __L4_MSG *msg)
{
	return __l4_recv(fr, msg, 0).ret;
}

static l4id_t l4_fork(cap_t tocap)
{
	return l4_sysux(MKWORD(L4_FORK, 0), MKWORD(tocap, 0), NULL, NULL);
}

static int l4_actv(cap_t tocid)
{
	return l4_sysux(MKWORD(L4_ACTV, 0), MKWORD(tocid, 0), NULL, NULL);
}

static ssize_t l4_cmap(cap_t capid, ulong moff, ulong vstart, size_t size)
{
	L4_MSG regs;
	regs.bx = moff;
	regs.di = vstart;
	regs.dx = size;
	return l4_sysux(MKWORD(L4_CMAP, 0), MKWORD(0, capid), &regs, NULL);
}
#if 0 // unimpl. api {{{
static int l4_call(int to, const __L4_MSG *arg, __L4_MSG *ret)
{
	return dbg_l4_sysux(MKWORD(L4_CALL, 0), MKWORD(to, 0), arg, ret);
}

static int l4_sendmsg(int to, const __L4_MSG *msg, size_t len)
{
	UT_REGS reg;
	reg.dx = (ulong)msg;
	return dbg_l4_sysux(MKWORD(L4_SEND, len), MKWORD(to, 0), &reg, NULL);
}

static int l4_recvmsg(int fr, __L4_MSG *msg, size_t len)
{
	UT_REGS reg;
	reg.dx = (ulong)msg;
	return dbg_l4_sysux(MKWORD(L4_RECV, len), MKWORD(fr, 0), &reg, NULL);
}

static int l4_retn(int to, const __L4_MSG *ret)
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
