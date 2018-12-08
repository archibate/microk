#pragma once

#include <inttypes.h>

void *sbrk(soff_t increment);
int brk(void *addr);
