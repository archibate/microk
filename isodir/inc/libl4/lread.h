#pragma once

#include <libl4/l4/defs.h>
#include <types.h>
#include <stddef.h>

#define l4_lread(fr, buf, size) l4_read_ex(fr, buf, size, NULL)
ssize_t l4_lread_ex(l4id_t fr, void *buf, size_t size, l4id_t *pfr);
