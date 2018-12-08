#include <l4/l4api.h>
#include <fs/fsdefs.h>
#include <fs/pathsvr.h>
#include <fs/server.h>
#include <fs/fsvrsimp.h>
#include <stddef.h>
#include <string.h>
#include <memory.h>
#include <errno.h>
#include <debug.h>
#include <numtools.h>
#include <l4ids.h>

static inline l4id_t open_inode(struct inode *inode, uint oflags)
{
	//l4_puts("open_inode");
	return inode->i_op->opensv(inode, oflags);
}

struct entry {
	const char *path;
	struct inode *inode;
} ents[12];

l4id_t open_path(const char *path, uint oflags)
{
	for (int i = 0; i < ARRAY_SIZEOF(ents); i++)
		if (!ents[i].path)
			return -ENOENT;
		else if (!strcmp(ents[i].path, path)) /*{ l4_puts(path);
			//l4_print(i);
			l4id_t svr = */
			return open_inode(ents[i].inode, oflags);
			/*l4_print(svr);
			return svr;
		}*/

	return -ENOENT;
}

void add_path(const char *path, struct inode *inode)
{
	static int i = 0;
	ents[i].path = path;
	ents[i].inode = inode;
	i++;
}

struct file_operations plain_fops = {
	.open = fsvrsimp_open_general,
	.getich = fsvrsimp_getich_plain,
	.read = fsvrsimp_read_plain,
	.write = fsvrsimp_write_noperm,
	.lseek = fsvrsimp_lseek_plain,
};

struct inode_operations general_iops = {
	.opensv = fsvrsimp_opensv_forking,
};

l4id_t iosvr_opensv(struct inode *inode, uint oflags)
{
	//l4_puts("iosvr_opensv");
	return inode->i_fsvr;
}

struct inode_operations iosvr_iops = {
	.opensv = iosvr_opensv,
};

void init_ftab(void)
{
	static struct inode myinodes[12];
	struct inode *myinode = myinodes;
#define NEW_SVR_FILE(name, svr) \
	myinode->i_op = &iosvr_iops; \
	myinode->i_fsvr = (svr); \
	myinode->priv_data = NULL; \
	myinode->i_size = -1L; \
	add_path(name, myinode); \
	myinode += 1;
#define NEW_FILE(name, data, len) \
	myinode->i_op = &general_iops; \
	myinode->i_fop = &plain_fops; \
	myinode->priv_data = (data); \
	myinode->i_size = (len); \
	add_path(name, myinode); \
	myinode += 1;
#define NEW_TXT_FILE(name, data) \
	NEW_FILE(name, data, strlen(data))
#define NEW_BIN_FILE(name, prog) \
	extern char _prog_beg_##prog[], _prog_end_##prog[]; \
	NEW_FILE(name, _prog_beg_##prog, _prog_end_##prog - _prog_beg_##prog)

	NEW_BIN_FILE("/bin/cat", cat);
	NEW_BIN_FILE("/bin/shell", shell);
	NEW_BIN_FILE("/sys/xterm", xterm);
	NEW_BIN_FILE("/bin/hello", hello);
	NEW_BIN_FILE("/bin/termctl", termctl);
	NEW_TXT_FILE("hello.txt", "Hello, World!\n");
	NEW_SVR_FILE("/dev/vmon0", L4ID_XTMSVR);
	NEW_SVR_FILE("/dev/mon0",  L4ID_COMSVR);
	NEW_SVR_FILE("/dev/kbd0",  L4ID_KBDSVR);
}

void path_server(void)
{
	init_ftab();

	while (1) {
		OPENER_ARGS oa;
		if (0 > fspsv_recv_opener(&oa, L4_ANY)) {
			dbg_printf("WARNING: path_server: fspsv_recv_opener() < 0\n");
			continue;
		}
		l4id_t svr = open_path(oa.path, oa.oflags);
		fspsv_reply_opener(&oa, svr);
	}
}
#if 0 // {{{
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
#endif // }}}
