#pragma once

#ifndef _SYS_PROC_NO_INC_SYS_TYPES
#include <sys/types.h>
#endif

void __attribute__((noreturn)) _exit(int res);
int execa(const char *path, const char *argl);
#ifdef _SYS_TYPES
pid_t fork(void);
pid_t getpid(void);
pid_t getppid(void);
#endif
