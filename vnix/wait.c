#include "unix.h"
#include <l4/ichipc.h>
#include <errno.h>
#include "secrets.h"

pid_t waitpid(pid_t pid, wstat_t *p_wstat, int options)
{
	if (options != 0)
		return -EINVAL;

	wstat_t wstat = l4_recvich_ex(pid, STG_WAIT, &pid);

	if (p_wstat)
		*p_wstat = wstat;

	return pid;
}
