#include <libl4/rwipc.h>
#include <libl4/lwripc.h>
#include <libl4/ichipc.h>
#include <pathsvr.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <pool.h>

static void init_fidpool();
l4id_t open_path(l4id_t cli, const char *path);
void path_server(void)
{
	init_fidpool();

	while (1) {
		char path[MAXPATH+1];
		l4id_t cli;
		ssize_t size = l4_read_ex(L4_ANY, path, MAXPATH, &cli);
		if (size < 0) {
			l4_puts("WARNING: path_server: bad read!\n");
			continue;
		}
		path[size] = 0;
		int ret = open_path(cli, path);
		l4_sendich_ex(cli, 2, ret, L4_REPLY);
	}
}

struct {
	const char *path;
	const char *contents;
} files[] = {
	{"hello.txt", "Hello, World!\n"},//TODO!!ramfs!!
};

static l4id_t file_instance(l4id_t cli, const char *contents);
l4id_t open_path(l4id_t cli, const char *path)
{
	for (int i = 0; i < ARRAY_SIZEOF(files); i++) {
		if (!strcmp(files[i].path, path)) {
			return file_instance(cli, files[i].contents);
		}
	}
	return -ENOENT;
}

POOL_TYPE(cap_t, ARRAY_SIZEOF(files) * 2) fidpool;

void init_fidpool(void)
{
	POOL_INIT(&fidpool);
	for (l4id_t i = 0; i < POOL_SIZE(&fidpool); i++)
		POOL_FREE(&fidpool, 16 + i);
}

static void __attribute__((noreturn)) subserver(l4id_t cli, const char *contents);
l4id_t file_instance(l4id_t cli, const char *contents)
{
	cap_t capt = POOL_ALLOC(&fidpool);
	l4id_t svr = l4_fork(capt);
	if (!svr)
		subserver(cli, contents);
	l4_actv(capt);
	return svr;
}

void __attribute__((noreturn)) subserver(l4id_t cli, const char *contents)
{
	int op;
again:
	op = l4_recvich(cli, 1);
	switch (op) {
	case FILE_READ  :  l4_write(cli, contents, strlen(contents)); break;
	//case FILE_LREAD : l4_lwrite(cli, contents, strlen(contents)); break;
	case FILE_WRITE : l4_sendich_ex(cli, 2, -EPERM,  L4_REPLY);   break;
	//case FILE_LWRITE: l4_sendich_ex(cli, 2, -EPERM,  L4_REPLY);   break;
	default         : l4_sendich_ex(cli, 2, -ENOSYS, L4_REPLY);   break;
	};
	goto again;
}
