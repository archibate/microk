#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

typedef struct NODE {
	struct NODE *l, *r;
	unsigned char val;
	unsigned cnt;
} NODE;


#if 0
void printhtree(NODE *node)
{
	if (node->l) {
		assert(node->r);
		printf("(");
		printhtree(node->l);
		printf(":");
		printhtree(node->r);
		printf(")");
	} else {
		printf("%d", node->val);
	}
}
#endif

int inbalancize(NODE *tree)
{
	if (!tree->l)
		return 1;
	int l = inbalancize(tree->l);
	int r = inbalancize(tree->r);
	if (l > r) {
		NODE *tmp = tree->l;
		tree->l = tree->r;
		tree->r = tmp;
	}
	return l + r;
}

static NODE q[514];

NODE *genhtree(void)
{
	NODE *po = q + 257;
	int m1, m2;
again:
	m1 = 513, m2 = 513;
	for (int i = 0; i < 513; i++) {
		if (q[i].cnt <= q[m1].cnt) {
			m2 = m1;
			m1 = i;
		} else if (q[i].cnt <= q[m2].cnt) {
			m2 = i;
		}
	}
	if (q[m2].cnt == 0xffffff)
		return &q[m1];

	po->cnt = q[m1].cnt + q[m2].cnt;
	q[m1].cnt = 0xffffff;
	q[m2].cnt = 0xffffff;
	po->l = &q[m1];
	po->r = &q[m2];
	po++;

	goto again;
}

void unserializq(FILE *f)
{
	int i;
	for (i = 0; i < 257; i++) {
		unsigned char val, cnt;
		val = fgetc(f);
		cnt = fgetc(f);
		if (cnt == 0xff || feof(f))
			break;
		//printf("[%d, %d]\n", val, cnt);
		q[i].r = q[i].l = NULL;
		q[i].val = val;
		q[i].cnt = cnt == 0xff ? 0xffffff : cnt;
	}
	for (; i < 514; i++) {
		q[i].r = q[i].l = NULL;
		q[i].cnt = 0xffffff;
	}
}

void uncomp(NODE *htree, FILE *f, FILE *fout)
{
	NODE *node = htree;
	char c = fgetc(f);
	int i = 0;

	while (!feof(f)) {
		//printf("%d", !!(c&(1<<i)));
		if (c & (1 << i++))
			node = node->r;
		else
			node = node->l;
		if (!node->l) {
			//printf(": %c\n", node->val);
			fputc(node->val, fout);
			node = htree;
		}
		if (i >= 8) {
			c = fgetc(f);
			i = 0;
		}
	}
}

int main(int argc, char **argv)
{
	const char *input = argc > 1 ? argv[1] : "/dev/stdin", *output = argc > 2 ? argv[2] : "/dev/stdout";
	char opt;

	FILE *f = fopen(input, "rb");
	if (!f) {
		fprintf(stderr, "cannot fopen(%s): %s\n", input, strerror(errno));
		return -1;
	}

	char magic[6];
	fread(magic, sizeof(magic), 1, f);
	if (memcmp(magic, "\x7fZCGEN", sizeof(magic))) {
		fprintf(stderr, "wrong file format: %s\n", input);
		return -1;
	}
	unserializq(f);
	NODE *ht = genhtree();
	inbalancize(ht);
	FILE *fout = fopen(output, "wb");
	uncomp(ht, f, fout);
	fclose(fout);
	fclose(f);
	return 0;
}
