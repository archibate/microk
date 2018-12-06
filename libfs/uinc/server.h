#pragma once

#include <fs/file.h>
#include <l4/l4defs.h>

l4id_t fs_serve(struct file *file, l4id_t cli);
