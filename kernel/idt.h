#pragma once

#include "gen.expentinfo.h"
#include "gen.irqentinfo.h"

void set_idt
	( unsigned int idx
	, void *addr
	, int dpl
	);

#define IRQ_IDT_BEG EXP_MAX
#define IDT_MAX 0x100
