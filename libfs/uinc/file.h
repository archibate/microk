#pragma once

#include <fs/fsdefs.h>
#include <l4/l4defs.h> // following in fs/inode.h

struct inode
{
	struct inode_operations *i_op;
	struct file_operations *i_fop;
	size_t i_size;
	void *priv_data;
	l4id_t i_fsvr;
};

struct inode_operations
{
	l4id_t (*opensv)(struct inode *inode, uint oflags);
};

struct file
{
	struct file_operations *f_op;
	struct inode *f_inode;
	off_t f_pos;
	size_t f_size;
	uint f_oflags;
};

struct file_operations
{
	int (*open)(struct file *file, struct inode *inode, uint oflags);
	ssize_t (*write)(struct file *file, const void *buf, size_t size);
	soff_t (*lseek)(struct file *file, soff_t offset, int whence);
	ssize_t (*read)(struct file *file, void *buf, size_t size);
	ich_t (*getich)(struct file *file);
};
