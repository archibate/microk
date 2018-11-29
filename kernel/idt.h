#pragma once

#include "gen.expentinfo.h"
#include "gen.irqentinfo.h"

void set_idt
	( unsigned int idx
	, void *addr
	, int dpl
	);

#define IRQ_IDT_BEG 0x20
#define IDT_MAX 0x100
