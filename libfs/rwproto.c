#include <l4/rwipc.h>
#include <l4/l4ipc.h>
#include <l4/ichipc.h>
#include <l4/capipc.h>
#include <errno.h>
#include "secrets.h"
#include <fs/file.h> // dirent
#include <fs/proto.h> // impelementation
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

	RC(l4_mkcap(FI_PRWCAP, addr & -PGSIZE, rw == FS_READ ? L4_ISRW : 0));
	RC(l4_sendcap(svr, STG_FSCAPS, FI_PRWCAP));

	return fi_waitreply(svr);
}

ssize_t fi_dorw(l4id_t svr, fscmd_t rw, ulong addr, size_t size)
{
	size_t size0 = size;
	while (size > 0)
	{
		size_t flen = (ssize_t)size < 0 ? PGSIZE : MIN(size, PGSIZE);
		ssize_t len = fi_dorwfrag(svr, rw, addr, flen);
		if (len < 0)
			return len;
		size -= len;
		addr += len;
		//if (len == 0)
		if (len < flen)
			return size0 - size;
	}
	return size0;
}

ssize_t fi_write(l4id_t svr, const void *buf, size_t size)
{
	return fi_dorw(svr, FS_WRITE, (ulong)buf, size);
}

ssize_t fi_read(l4id_t svr, void *buf, size_t size)
{
	return fi_dorw(svr, FS_READ, (ulong)buf, size);
}

soff_t fi_lseek(l4id_t svr, soff_t offset, int whence)
{
	FS_ARGS msg;
	msg.cmd = FS_LSEEK;
	msg.offset = offset;
	msg.whence = whence;
	fi_sendargs(&msg, svr);

	return fi_waitreply(svr);
}

ssize_t fi_pwrite(l4id_t svr, const void *buf, size_t size, off_t offset)
{
	int ret = fi_lseek(svr, offset, SEEK_SET);
	if (ret < 0)
		return ret;
	return fi_dorw(svr, FS_WRITE, (ulong)buf, size);
}

ssize_t fi_pread(l4id_t svr, void *buf, size_t size, off_t offset)
{
	int ret = fi_lseek(svr, offset, SEEK_SET);
	if (ret < 0)
		return ret;
	return fi_dorw(svr, FS_READ, (ulong)buf, size);
}

ich_t fi_getich(l4id_t svr)
{
	FS_ARGS msg;
	msg.cmd = FS_GETICH;
	fi_sendargs(&msg, svr);

	return fi_waitreply(svr);
}

int fi_readdir(l4id_t svr, struct direntry *ent)
{
	FS_ARGS msg;
	msg.cmd = FS_READDIR;
	fi_sendargs(&msg, svr);

	l4_read(svr, ent, sizeof(*ent));
	return fi_waitreply(svr);
}
