#pragma once

#include <fs/defs.h>
#include <libl4/l4/defs.h>

l4id_t fs_psvopensvr(l4id_t pathsvr, const char *path, uint oflags);
ssize_t fs_read(l4id_t svr, void *buf, size_t size);
ssize_t fs_write(l4id_t svr, const void *buf, size_t size);

#include <l4ids.h>
#define fs_opensvr(...) fs_psvopensvr(L4ID_PATHSVR, __VA_ARGS__)

#include <libl4/ichipc.h>
#define fs_sendcmd(svr, cmd) l4_sendich(svr, 1, cmd)
#define fs_recvcmd(fr)       l4_recvich(fr,  1)
#define fs_close(svr)        fs_sendcmd(svr, FS_CLOSE)
