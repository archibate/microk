#include <c4/api.h>
#include <string.h>
#include <sprintf.h>
#include <ioport.h>
#include "keysvr.h"
#include "keymap.h"
#include "vkeys.h"
#define CLIENT 1

static uint keyboard_getich(void);
void keyboard_server(void)
{
	TX_ARGS args;
	while (1)
	{
		c4_wait(CLIENT, NULL);
		args.tx_ich = keyboard_getich();
		c4_send(CLIENT, &args);
	}
}

static _Bool ctrl_on, shift_on, caps_on;

// https://baike.so.com/doc/7103239-7326232.html#7103239-7326232-6
uint keyboard_getich(void)
{
again:
	c4_wait(C4_ANY, NULL);
	uint scan = io_inb(0x60);
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
		if (vcs == '[' || vcs == ']' || ('@' <= vcs && vcs <= '_'))
			vc = vcs - '@';
	}
	if (caps_on && 'a' <= vc && vc <= 'z')
		vc -= 'a' - 'A';
	else if (caps_on && 'A' <= vc && vc <= 'Z')
		vc -= 'A' - 'a';
	return vc;
}
