// vim: fdm=marker
#include <struct.h>
#include <memory.h>
#include <stddef.h>
#include <types.h>
#include "print.h"
#include "ptregs.h"
#include "panic.h"
#include "pool.h"
#include "caps.h"
#include "clock.h"
#include "tcb.h"
#include <errno.h>

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
#define V_VOLATILE volatile
#define vpt      ((V_VOLATILE ulong*)0xffc00000)
#define vpd      ((V_VOLATILE ulong*)0xfffff000)
#define vregs    ((V_VOLATILE IF_REGS*)0x400000)
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

ulong forkpgd(void)
{
	ulong opgd = getcr3();
	ulong pgd = copiedpgd();
	setcr3(pgd);
	forkizevpd();
	setcr3(opgd);
	return pgd;
}
// }}}
//#define TPR
#define PMR
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
TCB T[MAXTASK], *curr;

void settask(TCB *t)
{
	//printf("settask: %p\n", t);
	setcr3(M[t->mid]);
	curr = t;
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
}*/
void on_timer(void)
{
	tprintf("on_timer()\n");
}// }}}
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
	if (!cap->valid || cap->is_mem)
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

int do_share(cap_t toid, cap_t capid, int granted)
{
	tprintf("do_share(%d,%d,%d)\n", toid, capid, granted);

	TCB *to = getcaptcb(toid);
	if (!to) { printf("-ESRCH\n"); return -ESRCH; }

	assert(to != curr);
	assert(to->state == BLOCKED);

	V_VOLATILE CAP *cap = &vregs->C[capid];
	if (!cap->valid)
	{ printf("-ENOCAP\n"); return -ENOCAP; }
	CAP c = *cap;
	if (granted)
		cap->valid = 0;
	//TCB *old = curr;
	settask(to);
	vregs->C[capid] = c;
	return 0;
}

int do_ipc(cap_t toid)
{
	tprintf("do_ipc(toid=%d)\n", toid);
	ticktsc();
	TCB *to = getcaptcb(toid);
	if (!to) { printf("-ESRCH\n"); return -ESRCH; }

	assert(to != curr);
	assert(to->state == BLOCKED);
	to->state = RUNNING;
	curr->state = BLOCKED;

	ulong mypte = vpt[0x400];
	settask(to);
	vpt[0x400] = mypte;
	invlpg((ulong)vregs);
	testp(i);
	return 0;
}

int do_actv(cap_t toid)
{
	tprintf("do_actv(%d)\n", toid);
	TCB *to = getcaptcb(toid);
	if (!to) return -ESRCH;

	assert(to->state == BLOCKED);
	to->state = RUNNING;
	curr->state = BLOCKED;

	ulong mypte = vpt[0x400];
	IF_REGS *myregs = tmpg(mypte & -4096L);
	settask(to);
	bcopy(myregs->needcpy, vregs->needcpy, sizeof(vregs->needcpy));
	return 0;
}

int do_fork(cap_t toid, uint mid)
{
	tprintf("do_fork(%d,%d)\n", toid, mid);
	TCB *to = &T[mid];

	setup_thr_cap((CAP*)&vregs->C[toid], to, 1);

	vregs->ax = 0;
	M[mid] = forkpgd();
	to->mid = mid;
	to->state = BLOCKED;
	return 7;
}

void syscall(uint ax, uint cx)
{
	int cl = cx & 0xff, ch = (cx >> 8) & 0xff;
	switch (ax)
	{
	case 0x0: vregs->ax = do_ipc(cl);  return;
	case 0x1: vregs->ax = do_actv(cl); return;
	case 0x2: vregs->ax = do_fork(cl, 1); return;
	case 0x3: vregs->ax = do_share(cl, ch, 0); return;
	case 0x4: vregs->ax = do_share(cl, ch, 1); return;
	case 0x5: vregs->ax = do_real (ch); return;
	case 0x8: printf("c4_print: cx=%d\n", cx); return;
	case 0x9: printf("halting...\n"); asm volatile ("cli; hlt");
	default : vregs->ax = -ENOSYS; return;
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
	.mem.size = 0x40000,
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

	curr = &T[0];
	curr->mid = 0;
	curr->state = RUNNING;
	setup_thr_cap((CAP*)&vregs->C[0], curr, 1);

	vregs->C[3] = kmem_cap;
	vregs->C[4] = vram_cap;

	move_to_user(0x10000000,      0x202,              0xfeed7f90      );
}
