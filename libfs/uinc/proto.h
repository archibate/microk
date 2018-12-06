#pragma once

#include <fs/fsdefs.h>
#include <l4/l4defs.h>

ssize_t fi_read(l4id_t svr, void *buf, size_t size);
ssize_t fi_write(l4id_t svr, const void *buf, size_t size);
ssize_t fi_dorwfrag(l4id_t svr, fscmd_t rw, ulong addr, size_t size);
ssize_t fi_dorw(l4id_t svr, fscmd_t rw, ulong addr, size_t size);
