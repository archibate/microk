// vim: fdm=marker
#include <struct.h>
#include <memory.h>
#include <stddef.h>
#include <types.h>
#include <c4/defs.h>
#include <errno.h>
#include "print.h"
#include "ptregs.h"
#include "panic.h"
#include "pool.h"
#include "caps.h"
#include "clock.h"
#include "tcb.h"
#include "irq.h"

// ppgpool: done {{{
POOL_TYPE(ulong, 1024) ppgpool;

ulong ppage(void)
{
	return POOL_ALLOC(&ppgpool);
}

void free_ppg(ulong pa)
{
	POOL_FREE(&ppgpool, pa);
}

void init_ppg(void)
{
	POOL_INIT(&ppgpool);
	for (ulong pa = 0x400000; pa < 0x400000 + POOL_SIZE(&ppgpool) * 4096; pa += 4096)
		POOL_FREE(&ppgpool, pa);
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
// }}}
void init_vpt(void) // {{{
{
	ulong *pgd = (ulong*)ppage();
	ulong *pgt = (ulong*)ppage();

	bzero(pgd, 4096);
	bzero(pgt, 4096);
	pgd[0]    = (ulong)pgt | 3;
	pgd[1023] = (ulong)pgd | 3 | 24;

	for (ulong pa = 0x0; pa < 0x400000; pa += 4096)
		pgt[pa >> 12] = pa | 3;

	pgt[0]    = 0;   // deny NULL pointer

	setcr3((ulong)pgd);
	enacr0(0x80000000);
} // }}}
#define V_VOLATILE //volatile
#define vpt      ((V_VOLATILE ulong*)0xffc00000)
#define vpd      ((V_VOLATILE ulong*)0xfffff000)
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
		vpd[pdi] = ppage() | 7;
		invlpg(-4096L&(ulong)&vpt[pti]);
		//bzero(&vpt[pti & 1023], 4096);
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
//#define TPR
//#define PMR
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

#define ANY     255
#define NIL     255
#define MAXTASK 128
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
	testp(t);
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
SEM *getcapsem(cap_t capid)
{
	V_VOLATILE CAP *cap = &vregs->C[capid];
	if (!cap->valid || cap->is_mem || !cap->is_sem)
		return 0;
	return cap->sem.sem;
}

TCB *getcaptcb(cap_t capid)
{
	V_VOLATILE CAP *cap = &vregs->C[capid];
	if (!cap->valid || cap->is_mem || cap->is_sem)
		return 0;
	return cap->thr.tcb;
}

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

int do_share(cap_t toid, cap_t capid, cap_t hiscid, int granted)
{
	tprintf("do_share(%d,%d,%d)\n", toid, capid, granted);
	assert(toid != 0);

	TCB *to = getcaptcb(toid);
	if (!to) return -ESRCH;

	assert(to->state == BLOCKED);

	V_VOLATILE CAP *cap = &vregs->C[capid];
	if (!cap->valid) return -ENOCAP;
	CAP c = *cap;
	if (granted)
		cap->valid = 0;
	settask(to);
	vregs->C[hiscid] = c;
	return 0;
}

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
	panic("schedule(): no runnable task!\n");
}
void on_timer(void)
{
	tprintf("on_timer()\n");
	//schedule();
}

#define getregp()     (vpt[0x400])
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
}

/*todo:int do_retnwait(void)//{{{
{
	TCB *from = vcurr->recving;
	if (!from)
		return -ENCALL;
	if (from->state != WAITRET)
		return -ENWAIT;
	tprintf("ipc_retnwait: returning\n");
	vcurr->recving = from->next;
	allocregp();
	from->state = RUNNING;
	settask(from);
	return 0;
}*///}}}

int do_send(cap_t toid)
{
	tprintf("do_send(toid=%d)\n", toid);
	ticktsc();

	assert(toid != 0);
	TCB *to = getcaptcb(toid);
	if (!to) return -ESRCH;

	if (to->state == ONRECV)
	{
		to->state = RUNNING;
		vpt[0x403] = to->winpte;
		invlpg((ulong)wregs);
		memcpy(wregs->needcpy, vregs->needcpy, sizeof(vregs->needcpy));
		settask(to);
	}
	else
	{
		vcurr->state = ONSEND;
		vcurr->next = to->recving;
		to->recving = vcurr;
		vcurr->winpte = vpt[0x400];
		if (to->state == RUNNING)
			settask(to);
		else
			schedule();
	}

	testp(s);
	return 0;
}

int do_wait(void)
{
	tprintf("do_wait()\n");
	ticktsc();

	TCB *from = vcurr->recving;
	if (from)
	{
		vcurr->recving = from->next;
		assert(from->state == ONSEND);
		from->state = RUNNING;
		vpt[0x403] = from->winpte;
		invlpg((ulong)wregs);
		memcpy(vregs->needcpy, wregs->needcpy, sizeof(vregs->needcpy));
	}
	else
	{
		vcurr->state = ONRECV;
		vcurr->winpte = vpt[0x400];
		schedule();
	}

	testp(w);
	return 0;
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
		vcurr->state = WAITRET;
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
	testp(c);
	return 0;
}

int do_wait(void)
{
	tprintf("do_wait\n");
	ticktsc();

	TCB *from = vcurr->recving;
	if (from) {
		tprintf("ipc_wait: direct recv\n");
		assert(from->state == ONSEND);
		from->state = WAITRET;
		fsetregp(from->regp);
	} else {
		tprintf("ipc_wait: block on recv\n");
		vcurr->state = ONRECV;
		schedule();
	}
	testp(w);
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
	if (from->state != WAITRET)
		return -ENWAIT;
	tprintf("ipc_retn: returning last\n");
	if (toid) vcurr->recving = from->next;
	allocregp();
	from->state = RUNNING;
	//settask(from);
	testp(r);
	return 0;
}*/ // }}}

int do_actv(cap_t toid)
{
	tprintf("do_actv(%d)\n", toid);
	TCB *to = getcaptcb(toid);
	if (!to) return -ESRCH;

	assert(to->state == BLOCKED);
	to->state = RUNNING;

	//ulong mypte = vpt[0x400];
	//IF_REGS *myregs = tmpg(mypte & -4096L);
	//settask(to);
	//bcopy(myregs->needcpy, vregs->needcpy, sizeof(vregs->needcpy));
	//untpmg(myregs);
	return 0;
}

TCB *irqsvr[IRQ_MAX];

int do_softirq(int irq)
{
	TCB *to = irqsvr[irq];
	if (!to)
		return -ENOCAP;
	if (to->state != ONRECV)
		return -ENWAIT;//XXX:use to->recving += irqhwcb!!!!

	to->state = RUNNING;
	settask(to);
	vregs->ax = 0;
	vregs->si = irq;
	tprintf("on_softirq(%d) returning\n", irq);
	return 0;
}

void on_hwirq(int irq)
{
	tprintf("on_hwirq(%d)\n", irq);
	irq_done(irq);
	if (irq == IRQ_TIMER)
		return;//xxx:on_timer();
	int ret = do_softirq(irq);
	if (ret && (irq == IRQ_KEYBOARD || irq == IRQ_MOUSE)) {
		io_inb(0x60);//XXX:TODO
	}
}

int do_fork(cap_t toid, uint mid)
{
	tprintf("do_fork(%d,%d)\n", toid, mid);
	assert(!M[mid]);

	TCB *to = &T[mid];
	to->mid = mid;
	to->state = BLOCKED;

	setup_thr_cap((CAP*)&vregs->C[toid], to, 1);

	ulong oldcr3 = getcr3();

	vregs->ax = 0;
	M[mid] = copiedpgd();
	TCB *fr = vcurr;
	setcr3(M[mid]);

	forkizevpd();
	setup_thr_cap((CAP*)&vregs->C[0],    to, 1);
	setup_thr_cap((CAP*)&vregs->C[toid], fr, 1);

	setcr3(oldcr3);
	return 7;
}

void syscall(uint ax, uint cx)
{
	uchar cl = cx & 0xff, ch = (cx >> 8) & 0xff;
	uchar al = ax & 0xff, ah = (ax >> 8) & 0xff;
	assert(vcurr->state == RUNNING);
	switch (al)
	{
	case C4_SEND : vregs->ax = do_send(cl/*, (const void*)vregs->dx, ah*/);  break;
	case C4_WAIT : vregs->ax = do_wait(  /*        (void*)vregs->dx, ah*/); break;
	case C4_FORK : vregs->ax = do_fork(cl, cl); break;
	case C4_ACTV : vregs->ax = do_actv(cl); break;
	case C4_SHARE: vregs->ax = do_share(cl, ch, ah, 0); break;
	case C4_GRANT: vregs->ax = do_share(cl, ch, ah, 1); break;
	case C4_REAL : vregs->ax = do_real (ch); break;
	case 0x17    : printf("c4_puts: %s\n", (const char*)cx); break;
	case 0x18    : printf("c4_print: cx=%d(%c)\n", cx, cx); break;
	case 0x19    : printf("halting...\n"); asm volatile ("cli; hlt");
	default : vregs->ax = -ENOSYS; break;
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

static CAP vram_cap = {
	.mem.va = 0xe0000000,
	.mem.size = 800 * 600,
	.mem.pte = 0xfd000000|7,
};

static CAP kmem_cap = {
	.mem.va = 0xa0000000,
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
	map_pages_in(0x10000000, (ulong)_initrd | 7, _initrd_end - _initrd);
	new_pages_in(0xfeed0000,                          0x8000          );

	irqsvr[IRQ_KEYBOARD] = vcurr = &T[0];
	vcurr->mid = 0;
	vcurr->state = RUNNING;
	setup_thr_cap((CAP*)&vregs->C[0], vcurr, 1);

	static SEM sem0;
	setup_sem_cap((CAP*)&vregs->C[6], &sem0, 1);

	vregs->C[3] = kmem_cap;
	vregs->C[4] = vram_cap;

	move_to_user(0x10000000,      0x202 + (3<<12),              0xfeed7f90      );
}
