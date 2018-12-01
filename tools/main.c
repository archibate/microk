#include <stdio.h>

int main(void)
{
	while (!feof(stdin)) {
		unsigned c = getchar();
		printf("%02X\n", c);
	}
}
