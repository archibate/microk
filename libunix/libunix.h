#pragma once

#include <libl4/l4/defs.h>
#include "unistd.h"

#define PATHSVR 1

extern l4id_t __libunix_svs[FILEMAX];
#define svs __libunix_svs
