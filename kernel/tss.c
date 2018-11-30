#include <stddef.h>
#include "ptregs.h"
#include "tss.h"
#include "desc.h"

TSS tss0;

void init_tss(void)
{
	tss0.ts_esp0 = 0x400000 + offsetof(IF_REGS, C);
	tss0.ts_ss0 = 0x0010;
	tss0.ts_iomb = offsetof(TSS, ts_iomaps);
	tss0.ts_iomaps[0] = 0xff; // for now

	struct {
		unsigned short limit;
		SEGDESC *gdt;
	} __attribute__((packed)) gdtr;
	asm volatile ("sgdt %0\n" :: "m" (gdtr) : "memory");
	gdtr.gdt[5] = SEG16(STS_T32A, (unsigned long) &tss0, sizeof(TSS), 0);
	gdtr.gdt[5].sd_s = 0;
	asm volatile ("ltr %%ax\n" :: "a" (5 * 8));
}
