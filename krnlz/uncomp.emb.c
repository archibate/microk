#define NULL ((void*)0)
typedef unsigned long size_t;

typedef struct NODE {
	struct NODE *l, *r;
	unsigned cnt;
	unsigned char val;
} NODE;

static int inbalancize(NODE *tree)
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

static NODE *genhtree(NODE *q)
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

static const char *unserializq(NODE *q, const char *src)
{
	int i;
	for (i = 0; i < 257; i++) {
		unsigned char val, cnt;
		val = *src++;
		cnt = *src++;
		if (cnt == 0xff)
			break;
		q[i].r = q[i].l = NULL;
		q[i].val = val;
		q[i].cnt = cnt == 0xff ? 0xffffff : cnt;
	}
	for (; i < 514; i++) {
		q[i].r = q[i].l = NULL;
		q[i].cnt = 0xffffff;
	}
	return src;
}

static void uncomp(NODE *htree, const char *src, const char *end, char *dst)
{
	NODE *node = htree;
	char c = *src++;
	int i = 0;

	while (src < end) {
		if (c & (1 << i++))
			node = node->r;
		else
			node = node->l;
		if (!node->l) {
			*dst++ = node->val;
			node = htree;
		}
		if (i >= 8) {
			c = *src++;
			i = 0;
		}
	}
}

int uncompress(const char *src, size_t size, char *dst)
{
	const char *end = src + size;
	if (*(short*)src != 0x5a7f) // \x7fZ
		return -1;
	src += 2;
	if (*(int*)src != 0x4e454743) // CGEN
		return -1;
	src += 4;
	NODE q[514];
	src = unserializq(q, src);
	NODE *ht = genhtree(q);
	inbalancize(ht);
	uncomp(ht, src, end, dst);
	return 0;
}
