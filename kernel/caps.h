#pragma once

#include <types.h>
#include <struct.h>
STRUCT(MEM_CAP)
{
	union {
		/*struct {
			uint p : 1;
			uint w : 1;
			uint u : 1;
			uint pwt : 1;
			uint pcd : 1;
			uint a : 1;
			uint d : 1;
			uint ps : 1;
			union {
				uint pat : 1;
				uint g : 1;
			};
			uint avl : 3;
			uint sel : 20;
		} __attribute__((packed));*/
		uint pgattr : 12;
		ulong pte;
	};
	ulong va;
	ulong size;
};

STRUCT(THR_CAP)
{
	struct {
		uint valid : 1;
		uint perm_rw : 1;
		uint mbz : 1;
		uint reserved : 29;
	} __attribute__((packed));
	struct TCB *tcb;
};

STRUCT(CAP)
{
	union {
		struct {
			uint valid : 1;
			uint is_rw : 1;
			uint is_mem : 1;
		} __attribute__((packed));
		uint raw[3];
		MEM_CAP mem;
		THR_CAP thr;
	};
};

static inline void __unused_func1(void)
{
	switch (0) {
	case sizeof(CAP) == 12:
	case 0: break;
	};
}

#define VALID_CAP(cap) ((cap)->valid)
