#include <libl4/rwipc.h>
#include <libl4/lwripc.h>
#include <libl4/ichipc.h>
#include <fs/defs.h>
#include <fs/proto.h>
#include <fs/psvproto.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <pool.h>

static void init_fidpool(void);
static void init_ftab(void);
l4id_t open_path_for(l4id_t cli, const char *path, uint oflags);
void path_server(void)
{
	init_ftab();
	init_fidpool();

	while (1) {
		OPENER_ARGS oa;
		if (0 > fspsv_recv_opener(&oa, L4_ANY)) {
			l4_puts("WARNING: path_server: fspsv_recv_opener() returns negative!\n");
			continue;
		}
		l4id_t svr = open_path_for(oa.cli, oa.path, oa.oflags);
		fspsv_reply_opener(&oa, svr);
	}
}

struct file;
typedef void file_server_t(struct file *file, l4id_t cli);
struct file {
	const char *path;
	const void *data;
	size_t size;
	file_server_t *server;
} files[12];

void fserver_of_contents(struct file *file, l4id_t cli)
{
	switch (fs_recvcmd(cli)) {
		case FS_READ  :  l4_write(cli, file->data, file->size);     break;
		case FS_LREAD : l4_lwrite(cli, file->data, file->size);     break;
		case FS_WRITE : l4_sendich_ex(cli, 2, -EPERM,  L4_REPLY);   break;
		case FS_LWRITE: l4_sendich_ex(cli, 2, -EPERM,  L4_REPLY);   break;
		default       : l4_sendich_ex(cli, 2, -ENOSYS, L4_REPLY);   break;
	};
}

#if 0
int stdout_wrmain(const char *buf, ssize_t size)
{
	if (size < 0)
		return size;

	int ret = l4_write(L4ID_COMSVR, buf, size);
	l4_print((ulong)buf);
	asm volatile ("cli; hlt");
	return ret;
}

void fserver_of_stdout(struct file *file, l4id_t cli)
{
	int ret;
	char buf[256];
	switch (fs_recvcmd(cli)) {
		case FS_WRITE : ret = stdout_wrmain(buf,   l4_read(cli, buf, sizeof(buf))); break;
		case FS_LWRITE: ret = stdout_wrmain(buf, l4_lwread(cli, buf, sizeof(buf))); break;
		case FS_READ  : ret = -EPERM;  break;
		case FS_LREAD : ret = -EPERM;  break;
		default       : ret = -ENOSYS; break;
	};
	fs_reply(ret);
}
#endif

#if 0
void fsvfop_on_write(struct file *file, l4id_t cli)
{
	ret = file->fops->readfg(file, buf, size);
}
#endif

static int fi = 0;

static void ftab_addfileof_devi(const char *path, file_server_t *server)
{
	files[fi].path = path;
	files[fi].data = 0;
	files[fi].size = 0;
	files[fi].server = server;
	fi++;
}

static void ftab_addfileof_stri(const char *path, const char *datstr)
{
	files[fi].path = path;
	files[fi].data = datstr;
	files[fi].size = strlen(datstr);
	files[fi].server = fserver_of_contents;
	fi++;
}

static void ftab_addfileof_prog(const char *path, const void *beg, const void *end)
{
	files[fi].path = path;
	files[fi].data = beg;
	files[fi].size = end - beg;
	files[fi].server = fserver_of_contents;
	fi++;
}

#define ftab_addfileof_PROG(path, name) \
	extern char _prog_beg_##name[], _prog_end_##name[]; \
	ftab_addfileof_prog(path, _prog_beg_##name, _prog_end_##name);

void init_ftab(void)
{
	ftab_addfileof_devi("/dev/stdout", fserver_of_stdout);
	ftab_addfileof_stri("hello.txt", "Hello, World!\n");
	ftab_addfileof_PROG("hello.bin", hello);
}


static l4id_t file_instance(l4id_t cli, struct file *file);
l4id_t open_path_for(l4id_t cli, const char *path, uint oflags)
{
	for (int i = 0; i < ARRAY_SIZEOF(files); i++) {
		if (files[i].path && !strcmp(files[i].path, path)) {
			return file_instance(cli, &files[i]);
		}
	}
	return -ENOENT;
}

POOL_TYPE(cap_t, ARRAY_SIZEOF(files) * 2) fidpool;

void init_fidpool(void)
{
	POOL_INIT(&fidpool);
	for (l4id_t i = 0; i < POOL_SIZE(&fidpool); i++)
		POOL_FREE(&fidpool, 128 + i);
}

l4id_t file_instance(l4id_t cli, struct file *file)
{
	cap_t capt = POOL_ALLOC(&fidpool);
	l4id_t svr = l4_fork(capt);
	if (!svr) while (1)
		file->server(file, cli);
	l4_actv(capt);
	return svr;
}
