#pragma once

#include <fs/fsdefs.h>
#include <fs/file.h>
#include <errno.h>

ich_t fsvrsimp_getich_plain(struct file *file);
l4id_t fsvrsimp_opensv_forking(struct inode *inode, uint oflags);
int fsvrsimp_open_general(struct file *file, struct inode *inode, uint oflags);
ssize_t fsvrsimp_read_plain(struct file *file, void *buf, size_t size);
soff_t fsvrsimp_lseek_plain(struct file *file, soff_t offset, int whence);
ssize_t fsvrsimp_read_by_getich(struct file *file, void *buf, size_t size);
static ssize_t fsvrsimp_read_noperm(struct file *file, void *buf, size_t size)
{ return -EPERM; }
static ssize_t fsvrsimp_write_noperm(struct file *file, const void *buf, size_t size)
{ return -EPERM; }
static soff_t fsvrsimp_lseek_unseekable(struct file *file, soff_t offset, int whence)
{ return -ESPIPE; }
