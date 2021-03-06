#include "string.h"

void strcpy(char *dst, const char *src)
{
	while ((*dst++ = *src++));
}

void strncpy(char *dst, const char *src, unsigned long n)
{
	while (n-- && (*dst++ = *src++));
}

void strcat(char *dst, const char *src)
{
	strcpy(dst + strlen(dst), src);
}

void strncat(char *dst, const char *src, unsigned long n)
{
	strncpy(dst + strlen(dst), src, n);
}

unsigned long strlen(const char *src)
{
	unsigned long res = 0;
	while (*src++)
		res++;
	return res;
}

unsigned long strnlen(const char *src, unsigned long n)
{
	unsigned long res = 0;
	while (n-- && *src++)
		res++;
	return res;
}

int strcmp(const char *dst, const char *src)
{
	int res = 0;
	while (!(res = *dst - *src))
		if (*dst++ == 0 || *src++ == 0)
			break;
	return res;
}

int strncmp(const char *dst, const char *src, unsigned long n)
{
	int res = 0;
	while (n-- && !(res = *dst - *src))
		if (*dst++ == 0 || *src++ == 0)
			break;
	return res;
}

int strfind(const char *src, char ch)
{
	int i = 0;
	while (*src) {
		if (*src == ch)
			return i;
		i++, src++;
	}
	return -1;
}

int strfindl(const char *src, char ch)
{
	int i = 0;
	while (*src) {
		if (*src == ch)
			return i;
		i++, src++;
	}
	return i;
}

int strnfind(const char *src, char ch, unsigned long n)
{
	int i = 0;
	while (n-- && *src) {
		if (*src == ch)
			return i;
		i++, src++;
	}
	return -1;
}

int strfindin(const char *src, const char *chrs)
{
	int i = 0;
	while (*src) {
		if (strfind(chrs, *src) != -1)
			return i;
		i++, src++;
	}
	return -1;
}

int strfindlin(const char *src, const char *chrs)
{
	int i = 0;
	while (*src) {
		if (strfind(chrs, *src) != -1)
			return i;
		i++, src++;
	}
	return i;
}

char *strskipin(const char *dst, const char *chrs)
{
	while (strfind(chrs, *dst) != -1)
		dst++;
	return (char *) dst;
}

int strchop(char *dst, const char *chrs)
{
	int i = 0;
	unsigned long len = strlen(dst) - 1;
	while (len > 0 && strfind(chrs, dst[len]) != -1) {
		dst[len--] = 0;
		i++;
	}
	return len + 1;
}

char *strtrim(char *dst, const char *chrs)
{
	dst = strskipin(dst, chrs);
	strchop(dst, chrs);
	return dst;
}
