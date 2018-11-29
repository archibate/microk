// vim: fdm=marker
#include <struct.h>
#include <memory.h>
#include <types.h>
#include "print.h"
#include "ptregs.h"
#include "panic.h"

#define PPGMAX 1024

ulong ppgstk[PPGMAX], *ppgsp = ppgstk;

ulong ppage(void)
{
	return *--ppgsp;
}

void free_ppg(ulong pa)
{
	*ppgsp++ = pa;
}

void init_ppg(void)
{
	for (ulong pa = 0x400000; pa < 0x400000 + PPGMAX * 4096; pa += 4096)
		free_ppg(pa);
}

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

#define vpt ((volatile ulong*)0xffc00000)
#define vpd ((volatile ulong*)0xfffff000)

#define getpgd() ((vpd[1023] & -4096L))

void init_vpt(void)
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
}

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

#define TMPGMAX 6
ulong tmpgvastk[TMPGMAX], *tmpgvasp = tmpgvastk;

void init_tmpg(void)
{
	for (ulong va = 0x120000; va < 0x120000 + TMPGMAX * 4096; va += 4096)
		*tmpgvasp++ = va;
}

void *tmpg(ulong pa)
{
	ulong va = *--tmpgvasp;
	map_page(va, pa | 3);
	return (void*)va;
}

void untmpg(void *v)
{
	ulong pte = unmap_page((ulong)v);
	assert(pte & 1);
	*tmpgvasp++ = (ulong)v;
}

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

#ifdef macker // {{{

// https://github.com/jserv/codezero/blob/master/docs/Codezero_Reference_Manual.pdf
l4_send   (to,   flags) := l4_ipc(to,   L4_NIL, flags);
l4_recieve(from, flags) := l4_ipc(L4_NIL, from, flags);

l4_ipc(to, from, flags) := { l4_send(to, flags); l4_recieve(from, flags); }

#endif // }}}
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
#endif // }}}
#define vregs   ((volatile IF_REGS*)0x400000)
#define oldregs ((volatile IF_REGS*)0x402000)

#define CLOCKS_PER_SEC 100
typedef ulong clock_t;
#define TIME0 4

#define RUNNING 1
#define BLOCKED 2
#define ONRECV  3
#define ONSEND  4
STRUCT(TCB)
{
	ulong pgd;
	clock_t timeout;
	uint state;
};

#define ANY     62
#define NIL     63
#define MAXTASK 62
TCB T[MAXTASK], *curr;

clock_t clock(void)
{
	uint eax, edx;
	asm volatile ("rdtsc" : "=a" (eax), "=d" (edx) ::);
	unsigned long long ns = eax + ((long long)edx << 32);
	return (clock_t)(ns / (1000000 / CLOCKS_PER_SEC));
}

void settask(TCB *t)
{
	//printf("settask: %p\n", t);
	setcr3(t->pgd);
	curr = t;
}
#include <rdtsc.h> // {{{
uint ticktsc(void)
{
	static uint last_tsc;
	uint tsc = rdtsc();
	uint res = tsc - last_tsc;
	last_tsc = tsc;
	return res;
}
#if 0
#define tprintf(...) printf(__VA_ARGS__)
#else
#define tprintf(...)
#endif
#if 1
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
#endif // }}}

#define LCMP(x, y) ((long)(x) - (long)(y))

void on_timer(void)
{
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
}

void do_ipc(uint to)
{
	tprintf("do_ipc(to=%d)\n", to);
	ticktsc();

	curr->state = BLOCKED;
	assert(T[to].state == BLOCKED);
	T[to].state = RUNNING;

	ulong mypte = vpt[0x400];
	settask(&T[to]);
	vpt[0x400] = mypte;
	invlpg((ulong)vregs);
	testp(i);
}

void do_rela(uint to)
{
	tprintf("do_rela(%d)\n", to);
	ticktsc();

	curr->state = BLOCKED;
	assert(T[to].state == BLOCKED);
	T[to].state = RUNNING;

	ulong mypte = vpt[0x400];
	IF_REGS *myregs = tmpg(mypte & -4096L);
	settask(&T[to]);
	bcopy(myregs, vregs, 4096);
	testp(r);
}

void do_fork(uint to)
{
	tprintf("do_fork(%d)\n", to);
	ticktsc();

	vregs->ax = 0;
	T[to].pgd = forkpgd();
	T[to].state = BLOCKED;
	T[to].timeout = clock() + TIME0;
	vregs->ax = 7;
	testp(f);
}

void syscall(uint ax, uint cx)
{
	uint to = cx & 0xff;
	uint fr = (cx >> 8) & 0xff;
	assert(to != ANY);
	switch (ax)
	{
	case 0x0: return do_ipc(to);
	case 0x1: return do_rela(to);
	case 0x2: return do_fork(to);
	case 0x8: printf("c4_print: cx=%d\n", cx); return;
	case 0x9: printf("halting...\n"); asm volatile ("cli; hlt");
	default : return;
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

extern void init_timer(int freq);
void init_sys(void)
{
	init_timer(CLOCKS_PER_SEC);

	T[0].pgd = getcr3();
	T[0].state = RUNNING;
	T[0].timeout = clock() + TIME0;
	curr = &T[0];

	new_page    (  0x400000);
	new_page    (  0x401000);
	map_pages_in(0x10000000, (ulong)_initrd | 7, _initrd_end - _initrd);
	//map_pages_in(0xa0000000, 0x00000000     | 7,      0x40000         );
	new_pages_in(0xfeed0000,                          0x8000          );
	map_pages_in(0xe0000000, 0xfd000000     | 7,      800 * 600       );

	move_to_user(0x10000000,      0x202,              0xfeed7f90      );
}
