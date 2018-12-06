#include <l4/rwipc.h>
#include <l4/ichipc.h>
#include <memory.h>

ssize_t l4_read_ex(l4id_t fr, void *buf, size_t size, l4id_t *pfr)
{
	long i;
	ssize_t wsize = l4_recvich_ex(fr, 8, &fr);
	if (wsize < 0)
		return wsize;
	if (pfr)
		*pfr = fr;
	l4_sendich(fr, 9, size);
	if (size > wsize) size = wsize;
	for (i = 0; i + L4_FRGSIZ < size; i += L4_FRGSIZ) {
		l4_recv(fr, 10 + (i % 6), buf);
	}
	char tmp[L4_FRGSIZ];
	l4_recv(fr, 11, tmp);
	memcpy(buf, tmp, size - i);
	return size;
}

ssize_t l4_write(l4id_t to, const void *buf, size_t size)
{
	long i;
	l4_sendich(to, 8, size);
	ssize_t rsize = l4_recvich(to, 9);
	if (rsize < 0)
		return rsize;
	if (size > rsize) size = rsize;
	for (i = 0; i + L4_FRGSIZ < size; i += L4_FRGSIZ) {
		l4_send(to, 10 + (i % 6), buf);
	}
	char tmp[L4_FRGSIZ];
	memcpy(tmp, buf, size - i);
	l4_send(to, 11, tmp);
	return size;
}
