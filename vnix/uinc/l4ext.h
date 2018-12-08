#pragma once

#include <l4/l4defs.h>

int opensvr(l4id_t svr, int oflags);
int opensvrat(int fd, l4id_t svr, int oflags);
