#include "unix.h"
#include <l4/l4api.h>

#define FORKCAP 120

static l4id_t curr_ppid;

l4id_t getpid(void)
{
	return l4_getid(0);
}

l4id_t getppid(void)
{
	return curr_ppid;
}

l4id_t fork(void)
{
	l4id_t old_pid = getpid();
	l4id_t cpid = l4_fork(FORKCAP);
	if (cpid > 0) {
		//l4_puts("parent!");
		l4_actv(FORKCAP);
	} else if (!cpid) {
		//l4_puts("child!");
		curr_ppid = old_pid;
	} else {
		//l4_puts("bad l4_fork!");
	}
	//l4_print(cpid);
	//l4_puts("vnix_fork returning!");
	return cpid;
}

#if 0
#define TMPDCAP   115
#define VPDCAP    116
#define RPGCAP    117
#define TPGCAP    118
#define TP2CAP    119
#define STG_FORSC 12

#define VPT    0xffc00000
#define VPD    0xfffff000
#define TMPD   0xe0000000
#define vpt    ((ulong*)VPT)
#define vpd    ((ulong*)VPD)
#define tmpd   ((ulong*)TMPD)

#define COW 512
void dofork(cap_t toid)
{
	l4_mktcb(toid);
	l4_mkcap(VPDCAP, VPD, L4_ISRW);
	l4_pgcpy(VPDCAP, TMPDCAP);
	l4_cmap(TMPDCAP, TMPD, L4_ISRW);
	for (int i = 1; i < 1024; i++) {
		tmpd[i] &= ~2; // readonly
		tmpd[i] |= COW; // and mark COW
	}
	l4_tcbpd(0,    TMPDCAP);
	l4_tcbpd(toid, TMPDCAP);
}
#endif
