#include <libl4/lwripc.h>
#include <libl4/ichipc.h>
#include <libl4/l4/api.h>
#include <libl4/capipc.h>
#include <memory.h>
#include <struct.h>

#define LWRCAP  15
#define LRDCAP  14
#define LRDTMPVA 0x4a0000

ssize_t l4_lwrite(l4id_t to, const void *buf, size_t size)
{
	ulong addr = (ulong)buf, vadr, voff;
	int i = 0;
again:
	vadr = addr & -PGSIZE;
	voff = addr & (PGSIZE-1);
	size_t flen = size < PGSIZE - voff ? size : PGSIZE - voff;

	LWR_MSG msg;
	msg.voff = voff;
	msg.size = flen;
	////l4_puts("l4_lwrite");
	//l4_print(msg.size);
	//l4_print(msg.voff);

	l4_send(to, 2, &msg);

	int ret = l4_mkcap(LWRCAP, vadr);
	if (ret < 0)
		return ret;
	ret = l4_sendcap(to, 6, LWRCAP);
	if (ret < 0)
		return ret;

	addr += flen;
	size -= flen;

	ret = l4_recvich(to, 5);
	if (ret < flen)
		return ret;
	i++;
	if ((ssize_t)size > 0)
		goto again;

	msg.voff = 0;
	msg.size = -1;
	////l4_puts("l4_lwrite sending EOLW");

	l4_send_ex(to, 2, &msg, L4_REPLY);
	return ret;
}

ssize_t l4_lwread_ex(l4id_t fr, void *buf, size_t size, l4id_t *pfr)
{ // TODO: lwread
	void *buf0 = buf;

	////l4_puts("lwread!!'n");
	LWR_CLI lwr;
	int ret;
again:
	////l4_puts("lwread!!'agaub");
	ret = l4_lwrpga_accept(&lwr, fr);
	//l4_print(ret);
	if (ret == -ELRWTERM) {
		////l4_puts("lwread: got EOLW");
		goto out;
	}
	else if (ret < 0)
		return ret;
	if (pfr)
		*pfr = ret;
	fr = ret;

	const void *p = l4_lwrpga_getptr(&lwr);
	size_t flen = lwr.size < size ? lwr.size : size;
	memcpy(buf, p, flen);
	l4_lwrpga_reply(&lwr, flen);

	buf += flen;
	size -= flen;

	if (flen < lwr.size)
		goto out;

	if ((ssize_t)size > 0)
		goto again;

out:
	return buf - buf0;
}
#if 0 // {{{
ssize_t l4_lwritepga(l4id_t to, const void *buf, size_t size)
{
	ulong addr = (ulong)buf;
	ulong vadr = addr & -PGSIZE;
	ulong voff = addr & (PGSIZE-1);
	size = size < PGSIZE - voff ? size : PGSIZE - voff;

	LWR_MSG msg;
	msg.voff = voff;
	msg.size = size;

	l4_send(to, &msg);

	int ret = l4_mkcap(LWRCAP, vadr);
	if (ret < 0)
		return ret;
	ret = l4_sendcap(to, LWRCAP);
	if (ret < 0)
		return ret;

	ret = l4_recvich(to);
	return ret;
}
#endif // }}}
l4id_t l4_lwrpga_accept(LWR_CLI *lwr, l4id_t fr)
{
	LWR_MSG msg;
	////l4_puts("l4_lwrpga_acceptL:recvfrom");
	//l4_print(fr);
	lwr->cli = l4_recv(fr, 2, &msg);

	if (lwr->cli < 0)
		return lwr->cli;
	lwr->voff = msg.voff;
	lwr->size = msg.size;
	////l4_puts("l4_lwrpga_accept");
	//l4_print(lwr->size);
	//l4_print(lwr->voff);
	if (lwr->size == -1)
		return -ELRWTERM;
	////printf("lwr: cli=%d, voff=%d, size=%d\n", lwr->cli, lwr->voff, lwr->size);
	if (lwr->voff > PGSIZE) {
		//l4_puts("WARNING: lwr.voff overflows out of one page\n");
		if ((signed)lwr->voff < 0)
			return (signed)lwr->voff;
		else
			return -EAGAIN;
	}
	if (lwr->voff + lwr->size > PGSIZE) {
		//l4_puts("WARNING: lwr.voff + lwr.size overflows out of one page\n");
		return -EAGAIN;
	}

	int ret = l4_recvcap(fr, 6, LRDCAP);
	if (ret < 0)
		return ret;
	ret = l4_cmap(LRDCAP, 0, LRDTMPVA, PGSIZE);
	if (ret < 0)
		return ret;
	return lwr->cli;
}

const void *l4_lwrpga_getptr(LWR_CLI *lwr)
{
	return (const void *)(LRDTMPVA + lwr->voff);
}

int l4_lwrpga_reply(LWR_CLI *lwr, long ret)
{
	return l4_sendich_ex(lwr->cli, 5, ret, L4_REPLY);
}
