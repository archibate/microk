#include <stdlib.h>
#include <stdio.h>

void __attribute__((noreturn)) abort(void)
{
	fprintf(stderr, "aborted\n");
	exit(1);
}
