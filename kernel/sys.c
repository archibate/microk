// vim: fdm=marker
#include <struct.h>
#include <memory.h>
#include <types.h>
#include "print.h"

STRUCT(SEM) // this struct is maintained by kernel
{
	uint rd : 12;
	uint wr : 12;
};

void wait_wr(SEM *sem)
{
	// XXX
}

void wait_rd(SEM *sem)
{
	// XXX
}

int sem_len(SEM *sem)
{
	return sem->wr - sem->rd;
}

void sem_init(SEM *sem)
{
	sem->rd = sem->wr = 0;
}

int sem_wr(SEM *sem)
{
	while (sem->wr == sem->rd - 1)
		wait_wr(sem);
	return sem->wr++;
}

int sem_rd(SEM *sem)
{
	if (sem->rd == sem->wr)
		wait_rd(sem);
	return sem->rd++;
}

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
	for (ulong pa = 0x120000; pa < 0x120000 + PPGMAX * 4096; pa += 4096)
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

void enacr0(ulong flags)
{
	ulong cr0;
	asm volatile ("mov %%cr0,%0" : "=r" (cr0) ::);
	asm volatile ("mov %0,%%cr0" :: "r" (cr0 | flags) : "memory");
}

#define vpt ((volatile ulong*)0xffc00000)
#define vpd ((volatile ulong*)0xfffff000)

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
		invlpg((ulong)&vpt[pti]);
	}
	vpt[pti] = pte;
	invlpg(va);
}

void new_page(ulong va)
{
	map_page(va, ppage() | 7);
}

ulong del_page(ulong va)
{
	uint pdi = va >> 22;
	uint pti = va >> 12;

	if (5 != (5 & vpd[pdi]))
		return 0;
	if (5 != (5 & vpt[pti]))
		return 0;
	free_ppg(vpt[pti] & -4096L);
	vpt[pti] = 0;
	return 1;
}

#define SEMMAX 16
SEM sems[SEMMAX];

ulong syscall(uint ax, uint cx, uint dx)
{
	ulong r = 0;
	switch (ax) {
	case 0x0:                sem_init(&sems[cx % SEMMAX]); break;
	case 0x1: while (dx--) r = sem_rd(&sems[cx % SEMMAX]); break;
	case 0x2: while (dx--) r = sem_wr(&sems[cx % SEMMAX]); break;
	case 0x3: r = sem_len(&sems[cx % SEMMAX]); break;
	case 0x4:     new_page     (cx);           break;
	case 0x5: r = del_page     (cx);           break;
	};
	return r;
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
	map_pages_in(0x10000000, (ulong)_initrd | 7, _initrd_end - _initrd);
	new_pages_in(0xfeed0000,                          0x10000         );
	map_pages_in(0xfd000000, 0xfd000000     | 7,      800 * 600       );
	move_to_user(0x10000000, 0x002, 0xfeedff90);
}
