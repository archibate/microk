#include <stdio.h>
#include <fcntl.h>

int main(const char *path)
{
#if 0
	FILE *fin = stdin;
	if (path[0]) {
		fin = fopen(path, "r");
		if (!fin) {
			fprintf(stderr, "cat: %s: No such file or directory\n", path);
			return 1;
		}
	}
#else
	openat(0, path, O_RDONLY);
#define fin stdin
#endif

	char buf[256];

	while (fgets(buf, sizeof(buf), fin))
		puts(buf);

	return 0;
}
