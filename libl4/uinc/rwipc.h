#pragma once

#include <l4/l4defs.h>
#include <stddef.h>

#define l4_read(fr, buf, size) l4_read_ex(fr, buf, size, NULL)
ssize_t l4_read_ex(l4id_t fr, void *buf, size_t size, l4id_t *pfr);
ssize_t l4_write(l4id_t to, const void *buf, size_t size);
