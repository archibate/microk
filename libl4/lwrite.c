#include <libl4/lwrite.h>
#include <libl4/lwrsvr.h>
#include <libl4/cappool.h>
#include <libl4/ichipc.h>
#include <libl4/l4ipc.h>
#include <memory.h>
#include <struct.h>

ssize_t l4_lwrite(l4id_t to, const void *buf, size_t size)
{
	ulong addr = (ulong)buf;

	LWR_MSG msg;
	msg.size = size;
	msg.voff = addr & (PGSIZE-1);
	int ret = l4_send(to, &msg);
	if (ret < 0)
		return ret;

	ulong vstart = addr & -PGSIZE;
	cap_t cap = l4_alloccap();
	int ret = l4_mkcap(cap, vstart, 0);
	if (ret < 0) {
		l4_freecap(cap);
		return ret;
	}
	ret = __l4_send(to, NULL, L4_ISCAP | L4_BLOCK, cap);
	if (ret < 0)
		return ret;

	ssize_t wsiz = l4_recvich(to);
	return wsiz;
}

l4id_t l4_lwrsvr_accept(LWR_CLI *lwr)
{
	LWR_MSG msg;
	l4id_t cli = l4_recv(to, &msg);
	if (cli < 0)
		return cli;

	lwr->size = msg.size;
	lwr->voff = msg.voff;
	if (lwr->voff & -PGSIZE) {
		l4_sendich_ex(lwr->cli, -EINVAL, 0);
		return -EAGAIN;
	}
	lwr->mpos = lwr->voff;
	lwr->cli = cli;

	lwr->cap = l4_alloccap();
	l4_recv_ret_t recv = __l4_recv(lwr->cli, NULL);
	if (!(recv.flags & L4_ISCAP)) {
		l4_sendich_ex(lwr->cli, -EINVAL, 0);
		return -EAGAIN;
	}
	return lwr->cli;
}

ssize_t l4_lwrsvr_romapfg(LWR_CLI *lwr, const void **pp)
{
	ulong base = l4mm_alloc_vapage();
	int ret = l4_cmap(lwr->cap, lwr->mpos & -PGSIZE, base, PGSIZE);
	if (ret < 0)
		return ret;
	off_t off = lwr->mpos & (PGSIZE-1);
	size_t size = PGSIZE - off;
	lwr->mpos += size;
	*pp = (const void*)(base + off);
	return size;
}

void l4_lwrsvr_release(LWR_CLI *lwr, ssize_t ret)
{
	l4_freecap(lwr->cap);
	l4_sendich_ex(lwr->cli, ret, 0);
}

ssize_t l4_lread_ex(l4id_t fr, void *buf, size_t size, l4id_t *pfr)
{
	LWR_CLI lwr;
	int ret = l4_lwrsvr_accept(&lwr);
	if (ret < 0)
		return ret;

	const void *frag = NULL;
	ssize_t fgsiz, rsize = 0;
	while (rsize < size && (fgsiz = l4_lwrsvr_romapfg(lwr, &frag))) {
		if (fgsiz < 0)
			return -fgsiz;
		if (rsize + fgsiz > size)
			fgsiz = size - rsize;
		memcpy(buf, frag, fgsiz);
		rsize += fgsiz;
	}
	return rsize;
}
