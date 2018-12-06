#pragma once

#include <fs/fsdefs.h>
#include <l4/l4defs.h>

struct file
{
	struct file_operations *f_op;
	const void *data;
	size_t size;
	l4id_t f_svr;
};

struct file_operations
{
	ssize_t (*write)(struct file *file, const void *buf, size_t size);
	ssize_t (*read)(struct file *file, void *buf, size_t size);
};
