#pragma once

#include <struct.h>
#include "l4defs.h"

STRUCT(TCB)
{
	uint mid : 8; // TODO: remove me // use tcb->pgd instead?
#define RUNNING 1
#define ONSEND  2
#define ONRECV  3
#define ONREPLY 4
#define BLOCKED 5
	uint state : 4;
	stage_t expstage : 4;
	l4id_t expecting : 16;
	ulong wtmpte;
};
