#include <stdio.h>
#include <string.h>

int main(char *cmd)
{
	int i = strfind(cmd, ' ');
	const char *arg = "0";
	if (i != -1) {
		cmd[i] = 0;
		arg = cmd+i+1;
	}
	if (!strcmp(cmd, "color")) {
		printf("\033[%sm", arg);
	} else if (!strcmp(cmd, "clear")) {
		printf("\033[2J");
	} else if (cmd) {
		fprintf(stderr, "termctl: command %s not understood\n", cmd);
	} else {
		fprintf(stderr, "usage: termctl <color|clear> [ansi color #]\n");
	}
	return 0;
}
