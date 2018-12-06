// vim: fdm=marker
#define YOUCAP
#define YOUSTG
//#define YOUGLB
//#define YOUSLT
//#define PMR
//#define TPR
#include <struct.h>
#include <memory.h>
#include <stddef.h>
#include <types.h>
#include <l4/defs.h>
#include <errno.h>
#include <pool.h>
#include "print.h"
#include "ptregs.h"
#include "panic.h"
#include "caps.h"
#include "clock.h"
#include "tcb.h"
#include "msg.h"
#include "irq.h"

// ppgpool: done {{{
POOL_TYPE(ulong, 1024) ppgpool;

ulong ppage(void)
{
	return POOL_ALLOC(&ppgpool);
}

void free_ppg(ulong pa)
{
	/*if (pa > 0x200000)
		bzero((void*)pa, 4096);*/
	POOL_FREE(&ppgpool, pa);
}

void init_ppg(void)
{
	POOL_INIT(&ppgpool);
	for (ulong pa = 0x400000; pa < 0x400000 + POOL_SIZE(&ppgpool) * 4096; pa += 4096)
		//POOL_FREE(&ppgpool, pa);
		free_ppg(pa);
}
// }}}
// mmu asm operations: done {{{
void invlpg(ulong va)
{
	asm volatile ("invlpg (%0)" :: "r" (va) : "memory");
}

void setcr3(ulong cr3)
{
	asm volatile ("mov %0,%%cr3" :: "r" (cr3) : "memory");
}

ulong getcr3(void)
{
	ulong cr3;
	asm volatile ("mov %%cr3,%0" : "=r" (cr3) :: "memory");
	return cr3;
}

void enacr0(ulong flags)
{
	ulong cr0;
	asm volatile ("mov %%cr0,%0" : "=r" (cr0) ::);
	asm volatile ("mov %0,%%cr0" :: "r" (cr0 | flags) : "memory");
}

void enacr4(ulong flags)
{
	ulong cr4;
	asm volatile ("mov %%cr4,%0" : "=r" (cr4) ::);
	asm volatile ("mov %0,%%cr4" :: "r" (cr4 | flags) : "memory");
}
// }}}
void init_vpt(void) // {{{
{
	ulong *pgd = (ulong*)ppage();
	ulong *pgt = (ulong*)ppage();

	bzero(pgd, 4096);
	bzero(pgt, 4096);
#ifdef YOUGLB
	pgd[0]    = (ulong)pgt | 3 | 256;
#else
	pgd[0]    = (ulong)pgt | 3;
#endif
	pgd[1023] = (ulong)pgd | 3 | 24;

	for (ulong pa = 0x0; pa < 0x400000; pa += 4096)
		pgt[pa >> 12] = pa | 3;

	pgt[0]    = 0;   // deny NULL pointer

	setcr3((ulong)pgd);
	enacr0(1<<31);
#ifdef YOUGLB
	enacr4(1<<7);
#endif
} // }}}
#define V_VOLATILE //volatile
#define vpt      ((V_VOLATILE ulong*)0xffc00000)
#define vpd      ((V_VOLATILE ulong*)0xfffff000)
#define vldt     ((V_VOLATILE SEGDESC*)0x405000)
#define vregs    ((V_VOLATILE IF_REGS*)0x400000)
#define wregs    ((V_VOLATILE IF_REGS*)0x403000)
#define vcurr    (vregs->C[0].thr.tcb)
#define getpgd() ((vpd[1023] & -4096L))
// map/new/del/unmap_page: remain to do {{{
void map_page(ulong va, ulong pte)
{
	uint pdi = va >> 22;
	uint pti = va >> 12;

	if (!(1 & vpd[pdi])) {
		ulong pgt = ppage();
		vpd[pdi] = pgt | 7;
		invlpg(-4096L&(ulong)&vpt[pti]);
	}
	vpt[pti] = pte;
	invlpg(va);
}

void new_page(ulong va)
{
	map_page(va, ppage() | 7);
}

ulong unmap_page(ulong va)
{
	uint pdi = va >> 22;
	uint pti = va >> 12;

	if (!(1 & vpd[pdi]))
		return 0;
	ulong pte = vpt[pti];
	if (!(1 & pte))
		return 0;
	vpt[pti] = 0;
	return pte;
}

ulong del_page(ulong va)
{
	ulong pte = unmap_page(va);
	if (!pte)
		return 0;
	free_ppg(pte & -4096L);
	vpt[va >> 12] = 0;
	return 1;
}
// }}}
// tmpgpool: done {{{
POOL_TYPE(ulong, 6) tmpgpool;

void init_tmpg(void)
{
	POOL_INIT(&tmpgpool);
	for (ulong va = 0x120000; va < 0x120000 + POOL_SIZE(&tmpgpool) * 4096; va += 4096)
		POOL_FREE(&tmpgpool, va);
}

void *tmpg(ulong pa)
{
	ulong va = POOL_ALLOC(&tmpgpool);
	map_page(va, pa | 3);
	return (void*)va;
}

void untmpg(void *v)
{
	ulong pte = unmap_page((ulong)v);
	assert(pte & 1);
	POOL_FREE(&tmpgpool, (ulong)v);
}
// }}}
// pgd operations: remain wuli {{{
void newpgd(void)
{
	ulong pgd = ppage();
	ulong *pd = tmpg(pgd);
	bzero(pd, 4096);
	pd[1023] = (ulong)pgd | 3 | 24;
	untmpg(pd);
}

ulong copiedpgd(void)
{
	ulong pgd = ppage();
	ulong *pd = tmpg(pgd);
	bcopy(vpd, pd, 4096);
	pd[1023] = pgd | 3 | 24;
	untmpg(pd);
	return pgd;
}

void forkizevpd(void)
{
	for (uint i = 1; i < 1023; i++) {
		if (vpd[i] & 1) {
			ulong pta = ppage();
			ulong *pt = tmpg(pta);
#if 1
			for (uint k = 0; k < 1024; k++) {
				if (vpt[i*1024+k] & 1) {
					ulong pga = ppage();
					ulong *pg = tmpg(pga);
					ulong opgva = 4096*(i*1024+k); 
					bcopy((ulong*)opgva, pg, 4096);
					untmpg(pg);
					pt[k] = pga | 7;
				}
			}
#else
			bcopy(&vpt[i*1024], pt, 4096);
#endif
			vpd[i] = pta | 7;
			untmpg(pt);
		}
	}
	setcr3(getcr3());
}
// }}}
uint ticktsc(void) // performence monitoring: done {{{
{
	static uint last_tsc;
	uint tsc = rdtsc();
	uint res = tsc - last_tsc;
	last_tsc = tsc;
	return res;
}
#ifdef TPR
#define tprintf(...) printf(__VA_ARGS__)
#else
#define tprintf(...)
#endif
#ifdef PMR
#define testp(x) do { ulong t = ticktsc(); clock_t c = clock(); printf(#x":%d:%d\n", c, t); ticktsc(); } while (0)
#else
#define testp(x)
#endif
#if 0
gcc -O0:
	0: 119.5 / 120.0
	1:  28.0 /  27.9
	2:   8.4 /   8.3
	3:  10.7 /  10.8
gcc -Os:
	0: 115.8 / 116.3
	1:  24.9 /  25.0
	2:   5.7 /   5.7
	3:   7.9 /   7.9
gcc -O3:
	0: 118.1 / 118.6
	1:  20.1 /  20.2
	2:   4.9 /   4.9
	3:   6.6 /   6.6
#if 0 // {{{
// theses functions, changes the `buffering page`: 0x400000
void recvregs(ulong pgd)
{
	ulong *pd  = tmpg(pgd);
	ulong *pt  = (ulong*)(pd[1] & -4096L);
	ulong pte  =         (pt[0] & -4096L);
	vpt[0x400] = pte;
	untmpg(pd);
}

void sendregs(ulong pgd)
{
	ulong *pd  = tmpg(pgd);
	ulong pgt  = pd[1] & -4096L;
	ulong *pt  = tmpg(pgt);
	pt[0]      = vpt[0x400] | 7;
	untmpg(pt);
	untmpg(pd);
}

void sdrvregs(ulong pgd)
{
	ulong *pd  = tmpg(pgd);
	ulong pgt  = pd[1] & -4096L;
	ulong *pt  = tmpg(pgt);
	ulong pte  = pt[0] & -4096L;
	pt[0]      = vpt[0x400];
	vpt[0x400] = pte;
	untmpg(pt);
	untmpg(pd);
}
#ifdef macker // {{{

// https://github.com/jserv/codezero/blob/master/docs/Codezero_Reference_Manual.pdf
l4_send   (to,   flags) := l4_ipc(to,   L4_NIL, flags);
l4_recieve(from, flags) := l4_ipc(L4_NIL, from, flags);

l4_ipc(to, from, flags) := { l4_send(to, flags); l4_recieve(from, flags); }

#endif // }}}
#endif // }}}
#endif // }}}

ulong idleM;
ulong M[MAXTASK];
TCB T[MAXTASK];

void settask(TCB *t)
{
	//printf("settask: %p\n", t);
	setcr3(M[t->mid]);
}

void print_cap_info(CAP *cap)
{
	if (!cap->valid)
		printf("invalidcap\n");
	else if (cap->is_mem)
		printf("memcap: va=%#p, pte=%#p, size=%#x\n", cap->mem.va, cap->mem.pte, cap->mem.size);
	else
		printf("thrcap: tcb=%#p, perm_rw=%d\n", &cap->thr.tcb, cap->thr.perm_rw);
}
/*#define LCMP(x, y) ((long)(x) - (long)(y)) {{{
#define TIME0 4

void on_timer(void)
{
	tprintf("on_timer()\n");
	ticktsc();

	clock_t t = clock();
	if (LCMP(t, curr->timeout) > 0) {
		curr->timeout = (t += TIME0);
		int id = -1;
		for (int i = 0; i < MAXTASK; i++) {
			if (T[i].state == RUNNING)
				if (LCMP(T[i].timeout, t) <= 0)
					t = T[i].timeout, id = i;
		}
		assert(id != -1);
		if (&T[id] != curr)
			settask(&T[id]);
	}
	estp(t);
}*/// }}}
// {{{
/*int do_recv(cap_t frid)
{
	if (frid == ANY) {
		curr->recving = 0;
	} else {
		curr->recving = getcaptcb(frid);
		if (!curr->recving)
			return -ENOCAP;
	}

	curr->state = ONRECV;
	ulong mypte = vpt[0x400];
	settask(curr->recving);
	vpt[0x400] = mypte;
	return 0;
}

int recving_match(TCB *recving)
{
	return (recving == curr || !recving);
}

int do_send(cap_t toid)
{
	TCB *to = getcaptcb(toid);
	if (!to)
		return -ENOCAP;

	assert(to->state == ONRECV && recving_match(to->recving));

	to->state = RUNNING;
	to->recving = 0;
	settask(to);
	return 0;
}*/
// }}}
TCB *getcaptcb(cap_t capid)
{
	V_VOLATILE CAP *cap = &vregs->C[capid];
	if (!cap->valid || cap->is_mem || cap->is_sem)
		return 0;
	return cap->thr.tcb;
}

TCB *getidtcb(l4id_t tid)
{
	if (tid > MAXTASK || tid < 0)
		return 0;
	TCB *tcb = &T[tid];
	if (tcb->state == 0)
		return 0;
	return tcb;
}

#if 1
l4id_t tidof(TCB *tcb)
{
	int i = tcb - T;
	if (i < MAXTASK && i >= 0)
		return i;
	else
		return tcb->mid;
}
#else
#define tidof(t) ((t)->mid)
#endif
#if 0 // {{{
int do_real(cap_t capid) // todo: will later to be removed, done in page fault instead
{
	tprintf("do_real(%d)\n", capid);
	//assert(getcaptcb(0)->state == BLOCKED);

	V_VOLATILE CAP *cap = &vregs->C[capid];
	if (!cap->valid || !cap->is_mem)
		return -ENOCAP;
	for ( ulong va = cap->mem.va, pte = cap->mem.pte;
		va < cap->mem.va + cap->mem.size;
		va += 4096, pte += 4096 )
		map_page(va, pte);
	return 0;
}
#endif // }}}

ssize_t do_cmap(cap_t capid, off_t moff, ulong vstart, size_t size)
{
	tprintf("do_cmap(%d,%#x,%#p,%#x)\n", capid, moff, vstart, size);

	moff &= -4096L;
	vstart &= -4096L;

	V_VOLATILE CAP *cap = &vregs->C[capid];
	if (!cap->valid || !cap->is_mem)
		return -ENOCAP;
	if (size > cap->mem.size - moff)
		size = cap->mem.size;
	tprintf("cap: pte=%#p, size=%#p\n", cap->mem.pte, cap->mem.size);
	for ( ulong va = vstart, pte = cap->mem.pte + moff;
		va < vstart + size;
		va += 4096, pte += 4096 )
		map_page(va, pte);
	return size;
}

int do_mkcap(cap_t capid, ulong va)
{
	tprintf("do_mkcap(%d,%#p)\n", capid, va);

	va &= -4096L;

	V_VOLATILE CAP *cap = &vregs->C[capid];
	if (!vpd[va >> 22])
		return -EFAULT;
	ulong pte = vpt[va >> 12];
	if ((pte & 5) != 5)
		return -EFAULT;
	cap->mem.pte = pte;
	cap->mem.size = 4096;
	tprintf("cap: pte=%#p, size=%#p\n", cap->mem.pte, cap->mem.size);
	return 0;
}

void __attribute__((noreturn)) do_idle(void);

void schedule(void)
{
	tprintf("schedule()\n");
	for (int i = 0; i < MAXTASK; i++) {
		if (T[i].state == RUNNING) {
			if (&T[i] != vcurr)
				settask(&T[i]);
			return;
		}
	}
	setcr3(idleM);
	do_idle();
	panic("schedule(): no runnable task!\n");
}
void on_timer(void)
{
	tprintf("on_timer()\n");
	//schedule();
}

#define getregp()     (vpt[0x400]) // {{{
static inline void allocregp(void)
{
	vpt[0x400] = ppage() | 7;
	invlpg((ulong)vregs);
}
static inline void setregp(ulong pte)
{
	vpt[0x400] = pte;
	invlpg((ulong)vregs);
}
static inline void fsetregp(ulong pte)
{
	free_ppg(vpt[0x400] & -4096L);
	fsetregp(pte);
} // }}}
/*todo:int do_retnrecv(void)//{{{
{
	TCB *from = vcurr->recving;
	if (!from)
		return -ENCALL;
	if (from->state != RECVRET)
		return -ENRECV;
	tprintf("ipc_retnrecv: returning\n");
	vcurr->recving = from->next;
	allocregp();
	from->state = RUNNING;
	settask(from);
	return 0;
}*///}}}

int is_expecting(l4id_t expecting, l4id_t id)
{
	assert(id >= 0);
	assert(expecting >= 0);
	if (id < MAXTASK && expecting == L4_ANY)
		return 1;
	if (expecting == id)
		return 1;
	//printf("wuwu(%d,%d)\n", expecting, id);
	return 0;
}

MSG *alloc_msgslot(MSG *msgs, l4id_t tid)
{
	int i;
#ifdef YOUSLT
	i = tid % MAXMSG;
	if (!msgs[i].is_valid)
		return &msgs[i];
#endif
	for (i = 0; i < MAXMSG; i++)
	{
		if (!msgs[i].is_valid)
			return &msgs[i];
	}
	panic("alloc_msgslot: Too many messages\n");
	return 0;
}

MSG *msgslots_find(MSG *msgs, l4id_t exptid)
{
	int i;
#ifdef YOUSLT
	if (exptid != L4_ANY)
	{
		i = exptid % MAXMSG;
		if (msgs[i].is_valid && is_expecting(exptid, msgs[i].tid))
			return &msgs[i];
	}
#endif

	for (i = 0; i < MAXMSG; i++)
	{
		if (msgs[i].is_valid && is_expecting(exptid, msgs[i].tid))
			return &msgs[i];
	}
	return 0;
}

int do_recv(l4id_t exptid, stage_t stg, cap_t capid)
{
	tprintf("%d:do_recv(%d)\n", tidof(vcurr), exptid);
	ticktsc();

	MSG *msg = msgslots_find(vregs->msgs, exptid);
	if (msg)
	{
#ifdef YOUSTG
		if (msg->stage != stg) {
			printf("\033[1;31mWARNING: recv: bad stage: %d!=%d\033[0m\n", msg->stage, stg);
			return -(ESTAGE + msg->stage);
		}
#endif

		tprintf("%d/%d:direct recv %d/%d\n", tidof(vcurr), exptid, msg - vregs->msgs, msg->tid);
		if (msg->flags & L4_BLOCK) {
			assert(T[msg->tid].state == ONSEND);
			T[msg->tid].state = RUNNING;
		}
#ifdef YOUCAP
		if (msg->flags & L4_ISCAP) {
			if (capid != 0)
				memcpy(&vregs->C[capid], &msg->dat.cap, sizeof(CAP));
		} else
#endif
			memcpy(&vregs->msg0dat, &msg->dat, sizeof(msg->dat));
		vregs->cx = msg->flags;
		msg->is_valid = 0;
		testp(w1);
		return msg->tid;
	}
	else
	{
		tprintf("block on recv\n");
		vcurr->state = ONRECV;
		vcurr->expecting = exptid;
#ifdef YOUSTG
		vcurr->expstage = stg;
#endif
		vregs->waitcap = capid;
		schedule();
		testp(w2);
		return 0;
	}
}
/*int do_accpt(l4id_t frid, cap_t capid) // {{{
{
	tprintf("do_accpt(%d,%d)\n", frid, capid);

	return do_recv(frid);
}

int do_share(l4id_t toid, cap_t capid, int grant, int block)
{
	tprintf("do_share(toid=%d,capid=%d,grant=%d,block=%d)\n", toid, capid, granted, block);

	TCB *to = getidtcb(toid);
	if (!to) return -ESRCH;

	V_VOLATILE CAP *cap = &vregs->C[capid];
	if (!cap->valid) return -ENOCAP;
	CAP c = *cap;
	if (grant)
		cap->valid = 0;

	return do_send(toid, block);
}*/ // }}}
int do_send(l4id_t toid, stage_t stg, uint flags, cap_t capid)
{
	tprintf("%d:do_send(toid=%d,flags=%d)(dx=%d)\n", tidof(vcurr), toid, flags, vregs->msg0dat.dx);
	ticktsc();

#ifdef YOUCAP
	if ((flags & L4_ISCAP) && !vregs->C[capid].valid)
		return -ENOCAP;
#endif

	TCB *to = getidtcb(toid);
	if (!to) return -ESRCH;
	assert(to != vcurr);

	l4id_t mytid = tidof(vcurr);
	if (to->state == ONRECV && is_expecting(to->expecting, mytid))
	{
#ifdef YOUSTG
		if (to->expstage != stg) {
			printf("\033[1;31mWARNING: send: bad stage: %d!=%d\033[0m\n", to->expstage, stg);
			return -(ESTAGE + to->expstage);
		}
#endif
		tprintf("direct send\n");
		assert(to->wtmpte);
		vpt[0x403] = to->wtmpte;
		invlpg((ulong)wregs);
		to->state = RUNNING;
#ifdef YOUCAP
		if (flags & L4_ISCAP) {
			memcpy(&wregs->C[wregs->waitcap], &vregs->C[capid], sizeof(CAP));
		} else
#endif
			memcpy(&wregs->msg0dat, &vregs->msg0dat, sizeof(MSGDAT));
		wregs->cx = flags;
		settask(to);
		testp(s1);
		return mytid;
	}
	else
	{
		tprintf("blocky on send\n");
		/*if (to->state == ONSEND)
			assert(to->sending != mytid);*/
		if (flags & L4_BLOCK)
			vcurr->state = ONSEND;
		assert(to->wtmpte);
		vpt[0x403] = to->wtmpte;
		invlpg((ulong)wregs);
		MSG *msg = alloc_msgslot(wregs->msgs, tidof(vcurr));
#ifdef YOUCAP // good!
		if (flags & L4_ISCAP) {
			memcpy(&msg->dat.cap, &vregs->C[capid], sizeof(CAP));
		} else
#endif
			memcpy(&msg->dat, &vregs->msg0dat, sizeof(vregs->msg0dat));
		msg->flags = flags;
#ifdef YOUSTG
		msg->stage = stg;
#endif
		msg->is_valid = 1;
		msg->tid = mytid;
		if (to->state == RUNNING)
			settask(to);
		else
			schedule();
		testp(s2);
		return 0;
	}
}

TCB *irqsvr[IRQ_MAX];

int do_softirq(int irq, uint dx)
{
	tprintf("do_softirq(%d,%d)\n", irq, dx);
	ticktsc();

	TCB *to = irqsvr[irq];
	if (!to)
		return -ENOCAP;

	l4id_t irqtid = L4_IRQ(irq);
	if (to->state == ONRECV && is_expecting(to->expecting, irqtid))
	{
		to->state = RUNNING;
		settask(to); // seem this slow downs a lot, how about to put irq servers in the ldt segment?
		vregs->ax = irqtid;
		vregs->msg0dat.dx = dx;
		testp(i1);
		return 0;
	}
	else
	{
		assert(to->wtmpte);
		vpt[0x403] = to->wtmpte;
		invlpg((ulong)wregs);
		MSG *msg = alloc_msgslot(wregs->msgs, irqtid);
		msg->dat.dx = dx;
		msg->flags = 0;
		msg->is_valid = 1;
		msg->stage = 0;
		msg->tid = irqtid;
		if (to->state == RUNNING)
			settask(to);
		else
			schedule();
		testp(i2);
		return 0;
	}
}

void __attribute__((noreturn)) do_idle(void)
{
	tprintf("do_idle() called\n");
	asm volatile (
		"mov %0, %%esp\n"
		"1:\nsti\nhlt\n"
		"cli\njmp 1b\n"
		:: "r" (&vregs->sp)
		: "memory");
	__builtin_unreachable();
}

void on_hwirq(int irq)
{
	//tprintf("on_hwirq(%d)\n", irq);
	irq_done(irq);
	if (irq == IRQ_TIMER)
		return;//xxx:on_timer();
	int ret = do_softirq(irq, io_inb(0x60));
	if (ret < 0)
		printf("oops(%d:%m)!\n", ret, ret);
}
/*int do_call(cap_t toid) // {{{
{
	tprintf("do_call(toid=%d)\n", toid);
	ticktsc();

	assert(toid != 0);
	TCB *to = getcaptcb(toid);
	if (!to) return -ESRCH;
	if (to->state == ONRECV) {
		tprintf("ipc_call: direct send\n");
		vcurr->state = RECVRET;
		ulong regp = getregp();
		to->state = RUNNING;
		settask(to);
		fsetregp(regp);
	} else {
		tprintf("ipc_call: block on send\n");
		vcurr->regp = getregp();
		vcurr->next = to->recving;
		to->recving = vcurr;
		vcurr->state = ONSEND;
		if (to->state == RUNNING)
			settask(to);
	}
	estp(c);
	return 0;
}

int do_recv(void)
{
	tprintf("do_recv\n");
	ticktsc();

	TCB *from = vcurr->recving;
	if (from) {
		tprintf("ipc_recv: direct recv\n");
		assert(from->state == ONSEND);
		from->state = RECVRET;
		fsetregp(from->regp);
	} else {
		tprintf("ipc_recv: block on recv\n");
		vcurr->state = ONRECV;
		schedule();
	}
	estp(w);
	return 0;
}

int do_retn(cap_t toid)
{
	printf("ipc_retn: %p\n", vcurr);
	tprintf("do_retn(toid=%d)\n", toid);
	ticktsc();

	TCB *from = / *toid ? getcaptcb(toid) : * /vcurr->recving;
	if (!from)
		return -ESRCH;
	if (from->state != RECVRET)
		return -EWBLOCK;
	tprintf("ipc_retn: returning last\n");
	if (toid) vcurr->recving = from->next;
	allocregp();
	from->state = RUNNING;
	//settask(from);
	estp(r);
	return 0;
}*/ // }}}
int do_actv(cap_t toid)
{
	tprintf("do_actv(%d)\n", toid);
	TCB *to = getcaptcb(toid);
	if (!to) return -ENOCAP;

	assert(to->state == BLOCKED);
	to->state = RUNNING;

	ulong mypte = vpt[0x400];
	settask(to);//xxx
	vpt[0x403] = mypte;
	invlpg((ulong)wregs);
	bcopy(&wregs->msg0dat, &vregs->msg0dat, sizeof(vregs->msg0dat));
	return 0;
}

l4id_t alloc_tid(void)
{
	for (l4id_t i = 0; i < MAXTASK; i++)
		if (!T[i].state)
			return i;
	panic("alloc_tid: Too much task!!\n");
}

int do_fork(cap_t tocid)
{
	tprintf("do_fork(%d)\n", tocid);
	l4id_t tid = alloc_tid();

	TCB *to = &T[tid];
	to->mid = tid;
	to->state = BLOCKED;

	ulong oldcr3 = getcr3();

	vregs->ax = 0;
	M[tid] = copiedpgd();
	setcr3(M[tid]);

	forkizevpd();
	setup_thr_cap((CAP*)&vregs->C[0],     to, 1);
	vcurr->wtmpte = vpt[0x400];
	memset(vregs->msgs, 0, sizeof(vregs->msgs));

	setcr3(oldcr3);
	setup_thr_cap((CAP*)&vregs->C[tocid], to, 1);
	return tid;
}

void syscall(uint ax, uint cx)
{
	uchar cl = cx & 0xff, ch = (cx >> 8) & 0xff;
	uchar al = ax & 0xff, ah = (ax >> 8) & 0xff;
	assert(vcurr->state == RUNNING);
	uchar sys = L4_AXSYS(al), stg = L4_AXSTG(al);
	switch (sys)
	{
	case L4_SEND : vregs->ax = do_send (cl, stg, ah, ch); break;
	case L4_RECV : vregs->ax = do_recv (cl, stg,     ch); break;
	case L4_FORK : vregs->ax = do_fork (cl             ); break;
	case L4_ACTV : vregs->ax = do_actv (cl             ); break;
	case L4_MKCAP: vregs->ax = do_mkcap(ch, vregs->msg0dat.di); break;
	case L4_CMAP : vregs->ax = do_cmap (ch, vregs->msg0dat.bx, vregs->msg0dat.di, vregs->msg0dat.si); break;
	case L4_PUTS : printf("%d:l4_puts: \033[1;32m%s\033[0m\n", tidof(vcurr), (const char*)cx);        break;
	case L4_PRINT: printf("%d:l4_print: cx=\033[1;32m%d=%#x(%c)\033[0m\n", tidof(vcurr), cx, cx);     break;
	case L4_HALT : printf("halting...\n"); do_idle();                                                 break;
	default      : printf("\033[1;31mWARNING: invalid syscall number: %d\033[0m\n", ax); vregs->ax = -ENOSYS; break;
	};
}

void __attribute__((noreturn)) move_to_user(ulong eip, uint eflags, uint esp)
{
	asm volatile (
			"movl %4, %%ds\n"
			"movl %4, %%es\n"
			"movl %4, %%fs\n"
			"movl %4, %%gs\n"
			"pushl %4\n" // ss
			"pushl %3\n" // esp
			"pushl %2\n" // eflags
			"pushl %1\n" // cs
			"pushl %0\n" // eip
			"iretl\n"
			:: "r" (eip)
			 , "r" (0x001b)
			 , "r" (eflags)
			 , "r" (esp)
			 , "r" (0x0023)
			);
	__builtin_unreachable();
}

void new_pages_in(ulong vbeg, ulong size)
{
	for (ulong off = 0; off < size; off += 4096)
		new_page(vbeg + off);
}

void map_pages_in(ulong vbeg, ulong ptebeg, ulong size)
{
	for (ulong off = 0; off < size; off += 4096)
		map_page(vbeg + off, ptebeg + off);
}

extern char _initrd[] __attribute__((aligned(4096))), _initrd_end[];
extern char _vsysrd[] __attribute__((aligned(4096))), _vsysrd_end[];

static CAP vram_cap = {
	.mem.size = 800 * 600,
	.mem.pte = 0xfd000000|7,
};

static CAP kmem_cap = {
	.mem.size = 0x400000,
	.mem.pte = 0x00000000|5,
};

extern void init_timer(int freq);
void init_sys(void)
{
	init_timer(CLOCKS_PER_SEC);

	M[0] = getcr3();

	new_page    (  0x400000);
	new_page    (  0x401000);
	new_page    (  0x402000);

	ulong oldpgd = getcr3();
	idleM = copiedpgd();
	setcr3(idleM);
	forkizevpd();
	setcr3(oldpgd);

	map_pages_in(0x10000000, (ulong)_initrd | 7, _initrd_end - _initrd);
	new_pages_in(0x10000000 + _initrd_end - _initrd,  0x8000          );
	map_pages_in(0xc0000000, (ulong)_vsysrd | 7, _vsysrd_end - _vsysrd);
	new_pages_in(0xc0000000 + _vsysrd_end - _vsysrd,  0x8000          );
	new_pages_in(0xfeed0000,                          0x8000          );

	set_irq_enable(IRQ_KEYBOARD, 1);
	irqsvr[IRQ_KEYBOARD] = &T[2];

	vcurr = &T[0];
	vcurr->mid = 0;
	vcurr->state = RUNNING;
	setup_thr_cap((CAP*)&vregs->C[0], vcurr, 1);

	vcurr->wtmpte = vpt[0x400];

	vregs->C[8] = kmem_cap;
	vregs->C[9] = vram_cap;

	move_to_user(0x10000000,      0x202 + (3<<12),              0xfeed7f90      );
}
