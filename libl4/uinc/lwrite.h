#pragma once

#include <libl4/l4/defs.h>

ssize_t l4_lwrite(l4id_t to, const void *buf, size_t size);
