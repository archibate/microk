#pragma once

#include <sys/types.h>

typedef int wstat_t;
pid_t waitpid(pid_t pid, wstat_t *p_wstat, int options);

#define WIFEXITED(wstat)   1
#define WEXITSTATUS(wstat) ((wstat) & 0xff)
