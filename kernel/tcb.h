#pragma once

#include <struct.h>
#include <c4/defs.h>

STRUCT(TCB)
{
	uint mid : 8;
#define RUNNING 1
#define ONRECV 2
#define ONSEND 3
#define WAITRET 4
#define BLOCKED 5
	uint state : 8;
	c4id_t expecting : 16;
	TCB *recving;
	TCB *next;
	ulong winpte;
};
