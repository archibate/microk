#include "unix.h"

static svalue_t safevals[MAX_SAFEVALS];

int setsafeval(uint idx, svalue_t val)
{
	if (idx > MAX_SAFEVALS)
		return -1;
	safevals[idx] = val;
	return 0;
}

svalue_t getsafeval(uint idx)
{
	if (idx > MAX_SAFEVALS)
		return 0;
	return safevals[idx];
}
