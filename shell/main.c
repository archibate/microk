#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readline.h"
#include <memory.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <atoi.h>

#define LINE_MAX 1024
#define CSPACE " \t\n"

int echo(const char *s)
{
	printf("%s\n", s);
	return 0;
}

int __attribute__((noreturn)) shexit(const char *val)
{
	exit(atoi(val));
}

void __attribute__((noreturn)) shexeca(const char *name, const char *arg)
{
	static char path[LINE_MAX];
	unsigned long len = strlen(name);
	memcpy(path, "/bin/", 5);
	memcpy(path + 5, name, len);
	path[5 + len + 4] = 0;

	int res = execa(path, arg);

	if (res == -ENOENT)
		fprintf(stderr, "sh: command not found: %s\n", name);
	else
		fprintf(stderr, "sh: execa(%s): %m\n", path, res);
	exit(res);
}

/*int start(char *cmdl) //{{{
{
	const char *arg = get_name_arg(cmdl);
	const char *name = cmdl;

	int pid = fork();

	if (!pid)
		shexecap(name, arg);

	static int started_nr = 0;
	printf("[%d] %d\n", ++started_nr, pid);

	return 0;
} //}}}*/

int run_cmd(char *cmdl)
{
	if (!cmdl[0])
		return 0;

	static struct command {
		char *name;
		int (*fun)(const char *arg);
	} shcmds[] = {
		//{"cd", chdir},
		//{"start", (void*)start},
		{"exit", shexit},
	};

	int i = strfindlin(cmdl, CSPACE);
	char *arg = strskipin(cmdl + i, CSPACE);
	const char *name = cmdl;
	cmdl[i] = 0;

	for (int i = 0; i < sizeof(shcmds) / sizeof(shcmds[0]); i++)
		if (!strcmp(shcmds[i].name, name))
			return shcmds[i].fun(arg);

	int pid = fork();

	if (!pid) {
		if ((i = strfindin(arg, "><")) != -1) {
			int c = arg[i-1];
			if ('0' <= c && c <= '9')
				c -= '0';
			else
				c = arg[i] == '>' ? 1 : 0;
			const char *file = strtrim(arg + i + 1, CSPACE);
			c = openat(c, file, arg[i] == '>' ? (O_WRONLY /*| O_CREAT | O_ACCEL*/) : O_RDONLY);
			if (c) {
				fprintf(stderr, "sh: %s: %m\n", file, c);
				exit(c);
			}
			arg[i] = 0;
			strchop(arg, CSPACE);
		}
		shexeca(name, arg);
	}

	wstat_t res;
	waitpid(pid, &res, 0);
	return WEXITSTATUS(res);
}


int main(const char *fname)
{
	FILE *fin = NULL;
	if (fname[0]) {
		fin = fopen(fname, "r");
		if (!fin) {
			fprintf(stderr, "sh: cannot open %s\n", fin);
			exit(-1);
		}
	}

	if (!fin)
		printf("\nOSYS shell v0.1 (C) archibate 2018\n\n");

	char *cmdl;
	if (fin)
		cmdl = malloc(LINE_MAX + 1);

	while (1) {
		if (!fin)
		{
			printf("> ");
			fflush(stdout);
			cmdl = readline();
			if (!cmdl)
				break;
		}
		else if (!fgets(cmdl, LINE_MAX, fin))
			break;

#if 0
		printf("You've just typed %s\n", cmdl);
#else
		run_cmd(strtrim(cmdl, CSPACE));
#endif
	}

	if (fin) {
		free(cmdl);
		fclose(fin);
	}

	return 0;
}
