#include <l4/rwipc.h>
#include <l4/l4ipc.h>
#include <l4/ichipc.h>
#include <l4/capipc.h>
#include <errno.h>
#include "secrets.h"
#include <numtools.h>

#define fi_sendargs(msg, svr)  l4_send(svr, STG_FSARGS, msg)
#define fi_waitreply(svr)      l4_recvich(svr, STG_FSREPLY)

//#define VOFF(x) ((x) & (PGSIZE-1))

ssize_t fi_dorwfrag(l4id_t svr, fscmd_t rw, ulong addr, size_t size)
{
	off_t voff = addr & (PGSIZE-1);

	if (voff + size > PGSIZE)
		return -EINVAL;

	FS_ARGS msg;
	msg.cmd = rw;
	msg.voff = voff;
	msg.size = size;
	fi_sendargs(&msg, svr);

	RC(l4_mkcap(FI_PRWCAP, addr & -PGSIZE));
	RC(l4_sendcap(svr, STG_FSCAPS, FI_PRWCAP));

	return fi_waitreply(svr);
}

ssize_t fi_dorw(l4id_t svr, fscmd_t rw, ulong addr, size_t size)
{
	while (size > 0)
	{
		size_t len = MIN(size, PGSIZE);
		fi_dorwfrag(svr, rw, addr, len);
		size -= len;
		addr += len;
	}
	return size;
}

ssize_t fi_write(l4id_t svr, const void *buf, size_t size)
{
	return fi_dorw(svr, FS_WRITE, (ulong)buf, size);
}

ssize_t fi_read(l4id_t svr, void *buf, size_t size)
{
	return fi_dorw(svr, FS_READ, (ulong)buf, size);
}
