#include "ipcmsgs.h"
#include <types.h>
#include <stddef.h>
#include <c4/api.h>
#include <irqidxs.h>
#include <ioport.h>
#include <vkeys.h>
#include "keymap.h"

static uint keyboard_getich(void);
void keyboard_server(void)
{
	ICH_MSG msg;
	while (1)
	{
		c4id_t cli = c4_wait(C4_ANY, &msg);
		msg.ich = keyboard_getich();
		c4_send(cli, &msg);
	}
}

static _Bool ctrl_on, shift_on, caps_on;

uint keyboard_getich(void)
// ref: https://baike.so.com/doc/7103239-7326232.html
{
	UT_REGS regs;
again:
	c4_wait(C4_IRQ(IRQ_KEYBOARD), &regs);
	uint scan = regs.dx;
	_Bool on = !(scan & 0x80);
	scan &= 0x7f;

	uint vc = keymap[scan];
	switch (vc) {
	case VK_CTRL :           ctrl_on = on;       goto again;
	case VK_SHIFT:          shift_on = on;       goto again;
	case VK_CAPS :  if (!on) caps_on = !caps_on; goto again;
	default      :  if (!on)                     goto again;
	};
	if (shift_on)
		vc = keymap_shift[scan];
	if (ctrl_on) {
		uint vcs = keymap_shift[scan];
		if ('{' <= vcs && vcs <= '}')
			vcs -= '{' - '[';
		if ('@' <= vcs && vcs <= '_')
			vc = vcs - '@';
	}
	if (caps_on && 'a' <= vc && vc <= 'z')
		vc -= 'a' - 'A';
	else if (caps_on && 'A' <= vc && vc <= 'Z')
		vc -= 'A' - 'a';
	return vc;
}
