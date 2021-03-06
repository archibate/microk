#include <fs/server.h>
#include <fs/fsvrsimp.h>
#include <l4/l4api.h>
#include <debug.h>
#include <asm/vinfo.h>
#include <memory.h>
#include <atoi.h>
#include <l4capids.h>
#include "asc16.h"

#define COL_HILIGHT 0x00008
#define COL_UNDERLN 0x10000
#define COL_LIGHTIN 0x20000

#define FGCOL(col) ((col) & 0xff)
#define BGCOL(col) (((col) >> 8) & 0xff)

void putfont
	( unsigned char *p
	, int stride
	, unsigned int col
	, char c
	)
{
	const unsigned char *f = asc16 + 16 * (unsigned char) c;
	unsigned char fgcol = FGCOL(col);
	unsigned char bgcol = BGCOL(col);

	for (int j = 0; j < ((col & COL_UNDERLN) ? 15 : 16); j++) {
		unsigned char w = f[j];
		for (int i = 0; i < 8; i++) {
			if (w & 0x80)
				p[i] = fgcol;
			else
				p[i] = bgcol;
			w <<= 1;
		}
		p += stride;
	}
	if (col & COL_UNDERLN) {
		memset(p, (col & 0xff), 8);
	}
}

void putcursor
	( unsigned char *p
	, int stride
	, unsigned int col
	, int beg
	, int end
	)
{
	unsigned char fgcol = FGCOL(col);
	unsigned char bgcol = BGCOL(col);

	p += beg * stride;
	for (int j = beg; j < end; j++) {
		for (int i = 0; i < 8; i++) {
			if (p[i] == fgcol)
				p[i] = bgcol;
			else
				p[i] = fgcol;
		}
		p += stride;
	}
}

void putblack
	( unsigned char *p
	, int stride
	, unsigned int col
	, int width
	)
{
	char c = BGCOL(col);
	for (int j = 0; j < 16; j++) {
		memset(p, c, width);
		p += stride;
	}
}

void moveright
	( unsigned char *p
	, int stride
	, int shift
	, int size
	)
{
	for (int j = 0; j < 16; j++) {
		memmove(p + shift, p, size);
		p += stride;
	}
}

static VIDEO_INFO video;
#define AT(x, y) (video.buf + video.xsiz * y + x), video.xsiz
#define ymax (video.ysiz - 16)
#define xmax (video.xsiz - 8)

static int x = 0, y = 0, old_x, old_y, cur_min = 0, cur_max = 3;
static unsigned int col = 0x000a;

void vmon_scroll_up(int h)
{
	if (h >= video.ysiz)
		h = video.ysiz;
	memcpy(video.buf, video.buf + video.xsiz * h, video.xsiz * (video.ysiz - h));
	bzero(video.buf + video.xsiz * (video.ysiz - h), video.xsiz * h);
}

void vmon_recursor(void);

#define putchar(c) //
void vmon_putc(char c)
{
	if (c == '\n')
	{
		putchar('\n');

		y += 16;
		x = 0;

		if (y > ymax) {
			vmon_scroll_up(y - ymax);
			y = ymax;
		}
	}
	else if (c == '\b')
	{
		putchar('\b');

		if (x >= 8) {
			moveright(AT(x, y), -8, xmax - x);
			x -= 8;
		}
	}
	else if (' ' <= c && c <= '~')
	{
		putchar(c);

		putfont(AT(x, y), col, c);
		x += 8;
	}
}

void vmon_setcolor(int num)
{
	switch (num) {
	case 0: col = 0x0007; return;
	case 1: col |= 0x0008; return;
	case 2: col |= 0x0800; return;
	case 4: col |= COL_UNDERLN; return;
	case 5: col |= COL_LIGHTIN; return;
	case 7: col ^= 0xffff; return;
	case 8: col &= 0xffff; return;
	}
	num -= 30;
	if (0 <= num && num < 8) {
		col = (col & ~0xff) | num;
		return;
	}
	num -= 10;
	if (0 <= num && num < 8) {
		col = (col & ~0xff00) | (num << 8);
		return;
	}
}

unsigned char rgb2pal(int r, int g, int b)
{
	r = (r * 21) / 256;
	g = (g * 21) / 256;
	b = (b * 21) / 256;
	return 16 + r + g * 6 + b * 36;
}

void vmon_writes(const char *s, const char *end)
{
	int num, num_old;

	putcursor(AT(x, y), col, 16 - cur_max, 16 - cur_min);

	while (s < end) {
		if (s[0] == '\033' && s[1] == '[') {
			num_old = num = 0;

			for (s += 2; s < end; s++) {
				if ('0' <= *s && *s <= '9')
					num = 10 * num + *s - '0';
				else if (*s == ';')
					num_old = num;

				else {
					switch (*s++) {
					case ':':
						num = strtol(s, &s, 16);
						col = rgb2pal((num>>16)&0xff, (num>>8)&0xff, num&0xff);
						if (*s == 'm') s++;
					case 'm':
						if (num_old)
							vmon_setcolor(num_old);
						vmon_setcolor(num);
						break;
					case 'A': y -= (num?num:1) * 16; if (y < 0) y = 0; break;
					case 'B': y += (num?num:1) * 16; if (y > ymax) y = ymax; break;
					case 'C': x += (num?num:1) * 8; if (x > xmax) y = xmax; break;
					case 'D': x -= (num?num:1) * 8; if (x < 0) x = 0; break;
					case 'a': moveright(AT(x, y), 8, (num?num:1) * 8); break;
					case 'J': vmon_scroll_up((num?num:1) * ymax); y = 0; break;
					case 'K': putblack(AT(x, y), col, xmax - x); break;
					case 'H': x = num; y = num_old; break;
					case 's': old_x = x; old_y = y; break;
					case 'u': x = old_x; y = old_y; break;
					case 'l': cur_min = 0; cur_max = 0; break;
					case 'h': cur_min = num_old; cur_max = (num ? num : 3); break;
					}
					break;
				}
			}
		} else vmon_putc(*s++);
	}

	//file_wr_flush(stdout);
	putcursor(AT(x, y), col, 16 - cur_max, 16 - cur_min);
}

ssize_t vmon_write(struct file *file, const void *buf, size_t size)
{
	vmon_writes(buf, buf + size);
	return size;
}

struct file_operations vmon_fops = {
	//.open = fsvrsimp_open_general,
	.write = vmon_write,
	.read = fsvrsimp_read_noperm,
	.getich = (void*)fsvrsimp_read_noperm,
	.lseek = fsvrsimp_lseek_unseekable,
};

struct file vmon_file = {
	.f_op = &vmon_fops,
};

int xterm_server(void *vram, void *vinfo_p)
{
	memcpy(&video, vinfo_p, sizeof(video));
	video.buf = vram;

#if 0
	l4_print(video.xsiz);
	l4_print(video.ysiz);
	l4_print((ulong)video.buf);
	x = y = 0;
	cur_min = 0;
	l4_print(x);
	l4_print(y);
	l4_print(cur_max);
	l4_puts("xterm!!!!");
#endif

	putcursor(AT(x, y), col, 16 - cur_max, 16 - cur_min);

	while (1)
		fs_serve(&vmon_file, L4_ANY);

	return 0;
}

int main(void)
{
	extern void init_palette(void);
	init_palette();

	l4_cmap(VRAM_CAP, 0x0, 0x90000000, -1L, L4_ISRW);
	l4_cmap(KMEM_CAP, 0x7000, 0x8ffff000, 0x1000, 0);
	return xterm_server((void*)0x90000000, (void*)0x8ffffb00);
}
