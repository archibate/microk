#include <unistd.h>
#include <stdio.h>

extern int main(const char *argl);
extern void init_heap(void); // malloc.c

FILE *stdin, *stdout, *stderr;

void __attribute__((noreturn)) exit(int res)
{
	fdclose(stdin);
	fdclose(stdout);
	fdclose(stderr);

	_exit(res);
}

void __attribute__((noreturn)) __crt_start(int __argpo, ...)
{
	char *argl = (char*)&__argpo;

	init_heap();

	stdin = fdopen(0, "r");
	stdout = fdopen(1, "w");
	stderr = fdopen(2, "w");

	exit(main(argl));
}
