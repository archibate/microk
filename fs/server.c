#include <fs/server.h>
#include <l4/capipc.h>
#include <l4/ichipc.h>
#include <l4/rwipc.h>
#include <struct.h>
#include "secrets.h"
#include <debug.h>

#define fs_validwrmsg(msg)   ((msg)->voff < PGSIZE && (msg)->voff + (msg)->size <= PGSIZE)
#define fs_validrdmsg(msg)   ((msg)->size <= PGSIZE)

#define fs_reply(to, ret)      l4_sendich_ex(to, STG_FSREPLY, ret, L4_REPLY)
#define fs_waitargs(msg, fr)   l4_recv(fr, STG_FSARGS, msg)

l4id_t fs_serve(struct file *file, l4id_t cli)
{
	FS_ARGS msg;
	ssize_t res;

	RC(cli = fs_waitargs(&msg, cli));

	if (msg.cmd == FS_WRITE || msg.cmd == FS_READ)
	{
		if (!fs_validwrmsg(&msg))
			return -EBADUS;

		RC(l4_recvcap(cli, STG_FSCAPS, FS_PRWCAP));
		RC(l4_cmap(FS_PRWCAP, 0x0, FS_PRWTMPADR, PGSIZE, msg.cmd == FS_READ ? L4_ISRW : 0));

		ulong addr = FS_PRWTMPADR + msg.voff;
		if (msg.cmd == FS_WRITE)
			res = file->f_op->write(file, (const void *) addr, msg.size);
		else
			res = file->f_op->read(file, (void *) addr, msg.size);
	}
	else if (msg.cmd == FS_LSEEK)
	{
		res = file->f_op->lseek(file, msg.offset, msg.whence);
	}
	else if (msg.cmd == FS_GETICH)
	{
		res = file->f_op->getich(file);
	}
	else if (msg.cmd == FS_READDIR)
	{
		struct direntry ent;
		res = file->f_op->readdir(file, &ent);
		l4_write(cli, &ent, sizeof(ent));
	}
	else
	{
		dbg_printf("WARNING: fs_serve: got invalid msg.cmd\n");
		return -EBADUS;
	}

	fs_reply(cli, res);
	return 0;
}
