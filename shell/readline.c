#include "readline.h"
#include <sprintf.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioext.h>
#include <asm/vkeys.h>
#include <stdarg.h>
#include <assert.h>

#define RL_BUFSIZE 1024

static void write1(const char *s)
{
	write(1, s, strlen(s));
}

#define getkbdch() getich(0)

struct rlbuf {
	struct rlbuf *next, *prev;
	char s[1];
};
#define LIST struct rlbuf
#define LMETHNAME rlb
#include <list.h>

static struct rlbuf head0 = { &head0, &head0, "" };
static struct rlbuf *history_head = &head0;

static void __rl_add_history(struct rlbuf *b)
{
	rlb_insert_after_ch(b, &history_head);
	history_head = b;
}

char *readline(void)
{
	size_t size = RL_BUFSIZE + offsetof(struct rlbuf, s);
	struct rlbuf *b = malloc(size);
	int i = 0, max = 0;
#define p b->s

	__rl_add_history(b);

	unsigned int c = getkbdch();
	if (c == EOF)
		return NULL;

	char s[8];
	
	while (c != '\n') {

		switch (c) {
		case '\b':
			if (i > 0) {
				write1("\b");
				i--;
				max--;
				memcpy(p + i, p + i + 1, max - i);
			}
			break;

		case VK_RIGHT:
			if (i < max) {
				write1("\033[C");
				i++;
			}
			break;

		case VK_LEFT:
			if (i > 0) {
				write1("\033[D");
				i--;
			}
			break;

		case VK_UP:
		case VK_DOWN:
			if (i) {
				sprintf(s, "\033[%dD\033[K", i);
				write1(s);
			} else	write1("\033[K");
			p[max] = 0;
			b = (c == VK_DOWN ? b->next : b->prev);
			max = strlen(p);
			write1(p);
			i = max;
			break;

		default:
			if (' ' <= c && c <= '~') {
				sprintf(s, "\033[%da%c", max - i, c);
				write1(s);
				memrcpy(p + i + 1, p + i, max - i);
				p[i++] = c;
				max++;
			}
			break;
		};

		assert(max < RL_BUFSIZE);

		c = getkbdch();
	}

	write1("\n");
	p[max] = 0;

	return p;
}

void rl_delete(char *buf)
{
	struct rlbuf *b = derive_of(struct rlbuf, s, buf);
	rlb_remove(b);
	free(b);
}
