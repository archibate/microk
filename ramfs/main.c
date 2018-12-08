#include <fs/pathsvr.h>
#include <fs/server.h>
#include <fs/fsvrsimp.h>
#include <l4/l4api.h>
#include <numtools.h>
#include <string.h>
#include <memory.h>
#include <alloca.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>

int fd0;

#define CLUSIZ  1024
#define FC0_BASE 0xc00
#define FAT_OFF (0x1000+FC0_BASE)
#define FAT_SIZ (0x0400+FC0_BASE)
#define ROT_OFF (0x1800+FC0_BASE)
#define DAT_OFF (0x4400+FC0_BASE)
#define MAXCLU  (FAT_SIZ*2/3)
#define INOMAX  256

clu_t fat[MAXCLU];
struct inode inodes[INOMAX];

void load_fat(void)
{
	static char img[FAT_SIZ];
	pread(fd0, img, FAT_SIZ, FAT_OFF);

	for (int i = 0, j = 0; i < MAXCLU * 2; i += 2, j += 3) {
		fat[i+0] = 0xfff & (( img[j+0]     )|( img[j+1] <<8 ));
		fat[i+1] = 0xfff & (( img[j+1] >>4 )|( img[j+2] <<4 ));
	}
}

#if 0 // {{{
extern struct super_operations ramfs_super_ops;
struct superblock ramfs_super = {
	.s_op = &ramfs_super_ops,
};
#endif // }}}

extern void setup_ramfs_root(struct inode *inode);
int main(void)
{
	fd0 = open("/dev/fd0", O_RDONLY);

	if (fd0 < 0) {
		fprintf(stderr, "/sys/ramfs: open(/dev/fd0): %m\n", fd0);
		return fd0;
	}

	load_fat();

	setup_ramfs_root(&inodes[0]);

	return 0;
}

extern struct file_operations  ramfs_fops;
extern struct inode_operations ramfs_iops;
extern struct file_operations  ramfs_root_fops;

void setup_ramfs_root(struct inode *inode)
{
	memset(inode, 0, sizeof(struct inode));
	inode->i_fop   = &ramfs_root_fops;
	inode->i_op    = &ramfs_iops;
}

int ramfs_root_open(struct file *file, struct inode *inode, uint oflags)
{
	file->f_cloff = 0;
	file->f_pos = 0;
	file->f_op = inode->i_fop;
	return 0;
}

ssize_t ramfs_root_read(struct file *file, void *buf, size_t size)
{
	return pread(fd0, buf, size, ROT_OFF + file->f_pos);
}

soff_t ramfs_root_lseek(struct file *file, soff_t offset, int whence)
{
	if (whence != SEEK_CUR)
		offset -= file->f_pos;
	if (whence == SEEK_END)
		offset += file->f_size;

	file->f_pos = MAX(0, MIN(offset, file->f_size - file->f_pos));

	return 0;
}

void setup_ramfs_inode(struct inode *inode)
{
	memset(inode, 0, sizeof(struct inode));
	inode->i_fop   = &ramfs_fops;
	inode->i_op    = &ramfs_iops;
}

ssize_t readclus(void *buf, int clus, int offset, size_t size)
{
	size = MAX(size, CLUSIZ - offset);
	return pread(fd0, buf, size, DAT_OFF + clus * CLUSIZ + offset);
}

#define clusnext(sb, c) fat[c]

ssize_t ramfs_read(struct file *file, void *buf, size_t size)
{
	size_t size0 = size = MIN(size, file->f_size - file->f_pos);

	while (size > 0)
	{
		ssize_t len = readclus(buf, file->f_clus, file->f_cloff, size);
		if (len < 0)
			return len;

		buf           += len;
		size          -= len;
		file->f_cloff += len;
		file->f_pos   += len;

		if (file->f_cloff >= CLUSIZ)
		{
			assert(file->f_cloff == CLUSIZ);

			file->f_clus = clusnext(file->f_sb, file->f_clus);
		}
	}

	return size0;
}

soff_t ramfs_lseek(struct file *file, soff_t offset, int whence)
{
	if (whence != SEEK_CUR)
		offset -= file->f_pos;
	if (whence == SEEK_END)
		offset += file->f_size;

	offset = MAX(0, MIN(offset, file->f_size - file->f_pos));

	if (file->f_pos > offset) {
		file->f_clus = file->f_inode->i_clus;
		file->f_pos = 0;
		file->f_cloff = 0;
	}

	while (file->f_pos <= offset)
	{
		ssize_t len = MAX(offset, CLUSIZ - file->f_cloff);

		file->f_cloff += len;
		file->f_pos   += len;

		if (file->f_cloff >= CLUSIZ)
			file->f_clus = clusnext(file->f_sb, file->f_clus);
	}

	return file->f_pos;
}

#define NAM_MAX 8
#define EXT_MAX 3
struct dos_entry
{
	char e_nam[NAM_MAX];
	char e_ext[EXT_MAX];
	uchar e_attr;
	uchar e_reserved;
	uchar e_ctime_10ms;
	dostime_t e_ctime;
	dosdate_t e_cdate;
	dosdate_t e_adate;
	ushort e_clus_hi;
	dostime_t e_mtime;
	dosdate_t e_mdate;
	ushort e_clus_lo;
	uint e_size;
} __attribute__((packed));

void init_inode_with_dos(struct inode *inode, struct dos_entry *dos)
{
	inode->i_clus = MKDWORD(dos->e_clus_lo, dos->e_clus_hi);
	inode->i_ctime_10ms = dos->e_ctime_10ms;
	inode->i_ctime = dos->e_ctime;
	inode->i_cdate = dos->e_cdate;
	inode->i_adate = dos->e_adate;
	inode->i_mtime = dos->e_mtime;
	inode->i_mdate = dos->e_mdate;
	inode->i_size  = dos->e_size;
	inode->i_eattr = dos->e_attr;
}

void get_dos_entry_name(char *name, struct dos_entry *dosent)
{
	char *ext = alloca(EXT_MAX + 1);
	memcpy(name, dosent->e_nam, NAM_MAX);
	memcpy(ext , dosent->e_ext, EXT_MAX);
	ext [EXT_MAX + 1] = 0;
	name[NAM_MAX + 1] = 0;

	size_t len = strchop(name, " ");
	size_t lex = strchop(ext, " ");
	char *p = name + len;
	if (lex) {
		*p++ = '.';
		memcpy(p, ext, EXT_MAX);
	}
}

#if 0 // {{{
struct inode *ramfs_open_inode(ino_t ino)
{
	if (ino > INOMAX)
		return NULL;
	return &inodes[ino];
}

void ramfs_close_inode(struct inode *inode)
{
	(void)inode;
}

struct super_operations ramfs_super_ops = {
	.open_inode  = ramfs_open_inode,
	.close_inode = ramfs_close_inode,
};
#endif // }}}

ino_t ramfs_alloc_ino(void)
{
	static ino_t all_ino = 0;
	assert(all_ino < INOMAX);
	return all_ino++;
}

int ramfs_readdir(struct file *file, struct direntry *ent)
{
	struct dos_entry dosent;
	int ret = file->f_op->read(file, &dosent, sizeof(dosent));
	if (ret < 0)
		return ret;

	get_dos_entry_name(ent->d_name, &dosent);

	ent->d_ino = ramfs_alloc_ino();
	struct inode *inode = &inodes[ent->d_ino];

	setup_ramfs_inode(inode);
	init_inode_with_dos(inode, &dosent);

	return 0;
}

struct file_operations ramfs_fops = {
	.open    = fsvrsimp_open_general,
	.write   = fsvrsimp_write_rofs,
	.readdir = ramfs_readdir,
	.lseek   = ramfs_lseek,
	.read    = ramfs_read,
};

struct file_operations ramfs_root_fops = {
	.open    = ramfs_root_open,
	.read    = ramfs_root_read,
	.write   = fsvrsimp_write_rofs,
	.lseek   = ramfs_root_lseek,
	.readdir = ramfs_readdir,
};

struct inode_operations ramfs_iops = {
	.opensv  = fsvrsimp_opensv_forking,
};
