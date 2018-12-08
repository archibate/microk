#pragma once

l4id_t fi_psvopensvr(l4id_t pathsvr, const char *path, fotype_t oflags);
#include <l4ids.h>
#define fi_opensvr(...)      fi_psvopensvr(L4ID_PATHSVR, __VA_ARGS__)
