#pragma once

#include <l4/l4defs.h>
#include "unistd.h"

#define PATHSVR 1

extern l4id_t __libunix_svs[FILEMAX];
#define svs __libunix_svs

#define FD_INRANGE(fd) ((fd) >= 0 && (fd) < FILEMAX)
