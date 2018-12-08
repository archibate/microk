#include <string.h>
#include <memory.h>
#include <malloc.h>
#include <sizet.h>

char *strdup(const char *s)
{
	size_t len = strlen(s) + 1;
	char *p = malloc(len);
	memcpy(p, s, len);
	return p;
}

char *strndup(const char *s, size_t n)
{
	size_t len = strnlen(s, n) + 1;
	char *p = malloc(len);
	memcpy(p, s, len);
	return p;
}
