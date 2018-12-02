#pragma once

#include <struct.h>
#include <l4/defs.h>

STRUCT(TCB)
{
	uint mid : 8; // TODO: remove me // use tcb->pgd instead?
#define RUNNING 1
#define ONRECV 2
#define ONSEND 3
#define WAITRET 4
#define BLOCKED 5
	uint state : 8;
	l4id_t expecting : 16;
	ulong wtmpte;
};
