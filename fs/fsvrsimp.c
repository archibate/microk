#include <fs/fsvrsimp.h>
#include <fs/server.h>
#include <string.h>
#include <numtools.h>
#include <memory.h>
#include <l4/l4api.h>

l4id_t fsvrsimp_opensv_forking(struct inode *inode, uint oflags)
{
	l4id_t tid = l4_fork(111);
	if (!tid) {
		struct file file;
		inode->i_fop->open(&file, inode, oflags);
		while (1)
			fs_serve(&file, L4_ANY);
	} else if (tid > 0)
		l4_actv(111);
	return tid;
}

int fsvrsimp_open_general(struct file *file, struct inode *inode, uint oflags)
{
	file->f_inode = inode;
	file->f_op = inode->i_fop;
	file->f_size = inode->i_size;
	file->f_clus = inode->i_clus;
	file->f_oflags = oflags;
	file->f_cloff = 0;
	file->f_pos = 0;
	return 0;
}

ssize_t fsvrsimp_read_plain(struct file *file, void *buf, size_t size)
{
	size = MIN(size, file->f_size - file->f_pos);
	memcpy(buf, file->f_inode->priv_data + file->f_pos, size);
	file->f_pos += size;
	//l4_print(size);
	return size;
}

ich_t fsvrsimp_getich_plain(struct file *file)
{
	if (file->f_pos >= file->f_size)
		return EOF;
	const char *data = file->f_inode->priv_data;
	return data[file->f_pos++];
}

soff_t fsvrsimp_lseek_plain(struct file *file, soff_t offset, int whence)
{
	if (whence == SEEK_CUR)
		offset += file->f_pos;
	if (whence == SEEK_END)
		offset += file->f_size;
	if (offset > file->f_size)
		offset = file->f_size;
	if (offset < 0)
		offset = 0;
	return file->f_pos = offset;
}

ssize_t fsvrsimp_read_by_getich(struct file *file, void *buf, size_t size)
{
	union {
		ich_t ich;
		char buf[sizeof(ich_t) + 1];
	} u;
	u.ich = file->f_op->getich(file);
	u.buf[sizeof(ich_t)] = 0;
	size = MIN(size, strlen(u.buf));
	memcpy(buf, u.buf, size);
	return size;
}
