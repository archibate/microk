#pragma once

#include <libl4/l4/defs.h>

cap_t l4_alloccap(void);
void l4_freecap(cap_t cap);
