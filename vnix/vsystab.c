#include "unix.h"

void *__attribute__((section(".vsystab"))) vsystab[] = {
	_exit, open, read, write, close, lseek, execa, fork, brk, sbrk, getpid, getppid, waitpid, getich, openat,
	getsafeval, setsafeval, pread, pwrite, readdir,
};

void _start(void)
{
	for (;;); // TODO!
}
