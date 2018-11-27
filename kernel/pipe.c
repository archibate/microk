#include <struct.h>
#include <sizet.h>
#include <page.h>
#include <psm.h>


STRUCT(PIPE) // this struct is maintained by kernel
{
	char *buf; // should pointing to a user space page
	unsigned rd : PGSHIFT;
	unsigned wr : PGSHIFT;
};

void pipe_init(PIPE *pipe)
{
	pipe->buf = (void *) alloc_ppage();
	pipe->rd = pipe->wr = 0;
}

void pipe_put(PIPE *pipe, char data)
{
	pipe->buf[pipe->wr++] = data;
}

char pipe_get(PIPE *pipe)
{
	return pipe->buf[pipe->rd++];
}

int pipe_len(PIPE *pipe)
{
	return pipe->wr - pipe->rd;
}

void pipe_readline(PIPE *pipe)
{
	while (pipe_len(pipe))
		pipe_get(pipe);
}


typedef unsigned int id_t;

unsigned long *init_task(unsigned long *stack, void (*start)(void))
{
	stack += STACK_SIZE - 16;
	stack[0] = (unsigned long) start;
	return stack;
}
