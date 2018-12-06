#pragma once

#include <fs/fsdefs.h>
#include <l4/l4defs.h>

STRUCT(OPENER_ARGS)
{
	l4id_t cli;
	uint oflags;
	char path[PATHMAX+1];
};

ssize_t fspsv_recv_opener(OPENER_ARGS *oa, l4id_t fr);
int fspsv_reply_opener(OPENER_ARGS *oa, l4id_t ret_svr);
