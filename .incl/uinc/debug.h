#pragma once

#ifndef DBG
#define dbg_printf(...)
#else
#include <stdio.h> // printf
#define dbg_printf(...) printf(__VA_ARGS__)
#endif

#define dbg_halt() do { asm volatile ("cli; hlt"); __builtin_unreachable(); } while (0)

struct IF_REGS;
void dbg_dumpifregs(struct IF_REGS *ifregs);
void dbg_backtrace(void);
