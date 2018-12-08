#pragma once

#include <fs/fsdefs.h>

#define STG_FSREPLY 5
#define STG_FSARGS  1
#define STG_FSCAPS  6
#define STG_FSWCAP  4

#define FS_PRWCAP    125
#define FS_WFCAP     97
#define FS_PRWTMPADR 0x70000000

#define FI_PRWCAP    125

#define RC(x) ({ int __$res = (x); if (__$res < 0) return __$res; })

STRUCT(FS_ARGS)
{
	fscmd_t cmd;
	union {
		struct {
			size_t size;
			off_t voff;
		};
		struct {
			int whence;
			off_t offset;
		};
	};
};
