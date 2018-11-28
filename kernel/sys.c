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

#define TMPGMAX 16
ulong tmpgvastk[TMPGMAX], *tmpgvasp = tmpgvastk;

void init_tmpg(void)
{
	for (ulong va = 0x120000; va < 0x120000 + TMPGMAX * 4096; va += 4096)
		*tmpgvasp++ = va;
}

ulong *tmpg(ulong pa)
{
	ulong va = *--tmpgvasp;
	map_page(va, pa | 3);
	return (ulong*)va;
}

void untmpg(ulong *v)
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

#define vregs ((volatile PT_REGS*)0x400000)

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

STRUCT(TCB)
{
	ulong *pgd;
	TCB *next, *prev;
};

#define MAXTASK 32
ulong pgds[MAXTASK];

void c4_ipc(uint to, uint fr)
{
	printf("c4_ipc(%d,%d)\n", to, fr);
	if (to != -1 && to == fr)
		sdrvregs(pgds[to]);
	else {
		if (to != -1) sendregs(pgds[to]);
		if (fr != -1) recvregs(pgds[fr]);
	}
}

void c4_swch(uint to)
{
	printf("c4_swch(%d)\n", to);
	setcr3(pgds[to]);
}

void c4_fork(uint to)
{
	printf("c4_fork(%d)\n", to);
	vregs->ax = 0;
	pgds[to] = forkpgd();
	vregs->ax = 7;
}

void syscall(uint ax, uint cx, uint dx)
{
	switch (ax)
	{
	case 0x0: return c4_ipc(cx, dx);
	case 0x1: return c4_swch(cx);
	case 0x2: return c4_fork(cx);
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

void init_rd(void)
{
	pgds[0] = getpgd();

	new_page    (  0x400000);
	map_pages_in(0x10000000, (ulong)_initrd | 7, _initrd_end - _initrd);
	map_pages_in(0xa0000000, 0x00000000     | 7,      0x40000         );
	new_pages_in(0xfeed0000,                          0x8000          );
	map_pages_in(0xe0000000, 0xfd000000     | 7,      800 * 600       );

	move_to_user(0x10000000,      0x002,              0xfeed7f90      );
}
