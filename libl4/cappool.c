#include <libl4/cappool.h>
#include <pool.h>

#define USED_CAPS_BEG

static POOL_TYPE(cap_t, USED_CAPS_BEG) cappool;
static int cappool_inited = 0;

static void l4_init_cappool(void)
{
	for (int cap = USED_CAPS_BEG; cap < MAXCAP; cap++)
		l4_freecap(cap);
	cappool_inited = 1;
}

cap_t l4_alloccap(void)
{
	if (!cappool_inited)
		l4_init_cappool();
	return POOL_ALLOC(&cappool);
}

void l4_freecap(cap_t cap)
{
	POOL_FREE(&cappool, cap);
}
