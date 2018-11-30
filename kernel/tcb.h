#pragma once

#include <struct.h>

STRUCT(TCB)
{
	uint mid;
#define RUNNING 1
#define ONRECV 2
#define ONSEND 3
#define BLOCKED 4
	uint state;
};
