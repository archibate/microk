#include <unistd.h>
#include <fcntl.h>

int main(void)
{
	int i = open("/dev/vmon0", O_WRONLY);
	if (i < 0)
		return i;
	const char buf[256] = "Hello, L4 microkernel!\n";
	write(i, buf, sizeof(buf));
	close(i);
	return 0;
}
