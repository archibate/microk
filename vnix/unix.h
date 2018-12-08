#pragma once

#include <l4/l4defs.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <sys/proc.h>
#include <sys/mm.h>
#include <sys/fops.h>
#include <sys/l4ext.h>
#include <sys/ioext.h>
#include <sys/memlay.h>
#include <sys/sval.h>

extern l4id_t unix_svs[FILEMAX];
#define svs unix_svs

#define FD_INRANGE(fd) ((fd) >= 0 && (fd) < FILEMAX)
