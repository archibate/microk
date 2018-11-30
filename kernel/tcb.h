#pragma once

#include <struct.h>

STRUCT(TCB)
{
	TCB *next;
	uint mid : 8;
#define RUNNING 1
#define ONRECV 2
#define ONSEND 3
#define WAITRET 4
#define BLOCKED 5
	uint state : 8;
	TCB *recving;
	ulong regp;
};
