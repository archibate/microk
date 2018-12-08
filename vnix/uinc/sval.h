#pragma once

#define MAX_SAFEVALS 32

typedef int svalue_t;

int setsafeval(uint idx, svalue_t val);
svalue_t getsafeval(uint idx);
