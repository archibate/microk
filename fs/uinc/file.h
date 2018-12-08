#pragma once

#include <fs/fsdefs.h>
#include <l4/l4defs.h> // followed in fs/inode.h
#include <sys/time.h>

struct superblock
{
	struct super_operations *s_op;
};

struct super_operations
{
	struct inode *(*open_inode)(struct superblock *sb, ino_t ino);
	void (*close_inode)(struct inode *inode);
};

struct inode
{
	struct inode_operations *i_op;
	struct file_operations *i_fop;
	void *priv_data;

	size_t    i_size;
	// iosvr specific:
	l4id_t    i_fsvr;
	// dos specific:
	dosmode_t i_eattr;
	clu_t     i_clus;
	uchar     i_ctime_10ms;
	dostime_t i_ctime;
	dosdate_t i_cdate;
	dosdate_t i_adate;
	dostime_t i_mtime;
	dosdate_t i_mdate;
};

struct inode_operations
{
	l4id_t (*opensv)(struct inode *inode, uint oflags);
};

#include <fs/dirent.h>

struct file
{
	struct file_operations *f_op;
	struct inode *f_inode;

	off_t  f_pos;
	size_t f_size;
	uint   f_oflags;
	// dos specific:
	clu_t  f_clus;
	off_t  f_cloff;
};

struct file_operations
{
	int (*open)(struct file *file, struct inode *inode, uint oflags);
	ssize_t (*write)(struct file *file, const void *buf, size_t size);
	soff_t (*lseek)(struct file *file, soff_t offset, int whence);
	ssize_t (*read)(struct file *file, void *buf, size_t size);
	int (*readdir)(struct file *file, struct direntry *ent);
	ich_t (*getich)(struct file *file);
};
