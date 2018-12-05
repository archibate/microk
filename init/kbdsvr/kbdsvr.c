#include <libl4/ipcmsgs.h>
#include <libl4/l4/api.h>
#include <asm/irqidxs.h>
#include <asm/ioport.h>
#include <asm/vkeys.h>
#include "keymap.h"

static uint keyboard_getich(void);
void keyboard_server(void)
{
	ICH_MSG msg;
	while (1)
	{
		l4id_t cli = l4_recv(L4_ANY, 1, &msg);
		msg.ich = keyboard_getich();
		msg.ic_raw[4] = 0;
		l4_send(cli, 2, &msg);
	}
}

static _Bool ctrl_on, shift_on, caps_on;

uint keyboard_getich(void)
// ref: https://baike.so.com/doc/7103239-7326232.html
{
	L4_MSG regs;
again:
	l4_recv(L4_IRQ(IRQ_KEYBOARD), 0, &regs);
	uint scan = regs.dx;
	_Bool on = !(scan & 0x80);
	scan &= 0x7f;

	uint vc = keymap[scan];
	switch (vc) {
	case VK_RCTRL : case VK_CTRL :  ctrl_on = on;       goto again;
	case VK_RSHIFT: case VK_SHIFT: shift_on = on;       goto again;
	case VK_CAPS  :        if (!on) caps_on = !caps_on; goto again;
	default       :        if (!on)                     goto again;
	};
	if (shift_on)
		vc = keymap_shift[scan];
	if (ctrl_on) {
		if (vc == '?')
			return 0x7f;
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
