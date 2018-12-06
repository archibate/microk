#include <l4/l4api.h>
#include <fs/fsdefs.h>
#include <fs/server.h>
#include <fs/pathsvr.h>
#include <stddef.h>
#include <string.h>
#include <memory.h>
#include <errno.h>
#include <pool.h>
#include <debug.h>
#include <l4ids.h>

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
			dbg_printf("WARNING: path_server: fspsv_recv_opener() returns negative!\n");
			continue;
		}
		l4id_t svr = open_path_for(oa.cli, oa.path, oa.oflags);
		fspsv_reply_opener(&oa, svr);
	}
}

//struct entry;
//typedef l4id_t file_server_t(struct file *file, l4id_t cli);
struct entry {
	const char *path;
	//file_server_t *server;
	struct file file;
} entries[12];

#if 0 // {{{
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
#endif // }}}
static int fi = 0;

ssize_t contentive_read(struct file *file, void *buf, size_t size)
{
	if (size > file->size)
		size = file->size;
	memcpy(buf, file->data, size);
	return size;
}

struct file_operations contentive_fops = {
	.read = contentive_read,
};

static void ftab_addfileof_devi(const char *path, l4id_t svr)
{
	entries[fi].path = path;
	entries[fi].file.data = 0;
	entries[fi].file.size = 0;
	entries[fi].f_svr = svr;
	fi++;
}

static l4id_t file_instance(l4id_t cli, struct file *file);
static void ftab_addfileof_stri(const char *path, const char *datstr)
{
	entries[fi].path = path;
	entries[fi].file.data = datstr;
	entries[fi].file.size = strlen(datstr);
	entries[fi].file.f_svr = file_instance(L4_ANY, &entries[fi].file);
	fi++;
}

static void ftab_addfileof_prog(const char *path, const void *beg, const void *end)
{
	entries[fi].path = path;
	entries[fi].file.data = beg;
	entries[fi].file.size = end - beg;
	entries[fi].file.f_svr = file_instance(L4_ANY, &entries[fi].file);
	fi++;
}

#define ftab_addfileof_PROG(path, name) \
	extern char _prog_beg_##name[], _prog_end_##name[]; \
	ftab_addfileof_prog(path, _prog_beg_##name, _prog_end_##name);

void init_ftab(void)
{
	ftab_addfileof_devi("/dev/stdout", L4ID_XTMSVR);
	ftab_addfileof_stri("hello.txt", "Hello, World!\n");
	ftab_addfileof_PROG("hello.bin", hello);
}


l4id_t open_path_for(l4id_t cli, const char *path, uint oflags)
{
	for (int i = 0; i < ARRAY_SIZEOF(entries); i++) {
		if (entries[i].path && !strcmp(entries[i].path, path)) {
			return file_instance(cli, &entries[i].file);
		}
	}
	return -ENOENT;
}

POOL_TYPE(cap_t, ARRAY_SIZEOF(entries) * 2) fidpool;

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
		fs_serve(file, cli);
	l4_actv(capt);
	return svr;
}
