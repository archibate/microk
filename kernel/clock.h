#pragma once

#include <rdtsc.h>

#define CLOCKS_PER_MSEC 100
#define CLOCKS_PER_SEC (1000*CLOCKS_PER_MSEC)

typedef ulong clock_t;

static inline clock_t clock(void)
{
	uint eax, edx;
	asm volatile ("rdtsc" : "=a" (eax), "=d" (edx) ::);
	unsigned long long ns = eax + ((long long)edx << 32);
	return (clock_t)(ns / (1000000 / CLOCKS_PER_MSEC));
}
