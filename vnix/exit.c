#include "unix.h"
#include <l4/ichipc.h>
#include "secrets.h"

void __attribute__((noreturn)) _exit(int res)
{
	wstat_t wstat = WEXITSTATUS(res) /* | WXXXX(xxx) */;

	pid_t ppid = getppid();
	while (1)
		l4_sendich(ppid, STG_WAIT, wstat);
}
