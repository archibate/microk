#include "irq.h" // set_irq_enable
#include <ioport.h>


void init_keyboard
(void)
{
	set_irq_enable(IRQ_KEYBOARD, 1);
}
