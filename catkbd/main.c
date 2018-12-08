#include <unistd.h>
#include <fcntl.h>
#include <sys/ioext.h>
#include <string.h>

int main(void)
{
	int i = open("/dev/vmon0", O_WRONLY);
	if (i < 0)
		return i;
	int j = open("/dev/kbd0", O_WRONLY);
	if (j < 0)
		return j;
	char buf[256] = "Now type something please:\n";
	write(i, buf, strlen(buf));

	while (1) {
		buf[4] = 0;
		if (EOF == (*(ich_t*)buf = getich(j)))
			break;
		write(i, buf, strlen(buf));
	}

	close(i);
	close(j);
	return 0;
}
