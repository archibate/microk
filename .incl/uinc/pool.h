#pragma once

#include <stddef.h>

#define POOL_TYPE(type, size) struct { \
	type *_sp; \
	type _stk[size]; \
}

#define POOL_INIT(pool)    ((void)((pool)->_sp = (pool)->_stk))
#define POOL_ALLOC(pool)   (*--(pool)->_sp)
#define POOL_FREE(pool, x) ((void)(*(pool)->_sp++ = (x)))
#define POOL_SIZE(pool)    (ARRAY_SIZEOF((pool)->_stk))
