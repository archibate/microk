#include <irq.h>
#include <ioport.h>


#define PIT_CTR0 0x40
#define PIT_CTR1 0x41
#define PIT_CTR2 0x42
#define PIT_CNTL 0x43


void init_timer(int freq)
{
	unsigned value = 11931180 / freq;

	io_outb(PIT_CNTL, 0x36);
	io_outb(PIT_CTR0, value & 0xff);
	io_outb(PIT_CTR1, (value >> 8) & 0xff);
	//io_outb(PIT_CTR2, (value >> 16) & 0xff);

	set_irq_enable(IRQ_TIMER, 1);
}


extern void on_timer(void);
void do_timer(void)
{
	irq_done(IRQ_TIMER);
	on_timer();
}
