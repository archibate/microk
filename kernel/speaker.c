#include <types.h>
#include <asm/ioport.h>

static void speaker_phase(uint32_t freq)
{
	uint32_t div;

	/* set the PIT 2 to the desired frequency */
	div = 1193180 / freq;
	io_outb(0x43, 0xb6);
	io_outb(0x42, (uint8_t) (div));
	io_outb(0x42, (uint8_t) (div >> 8));
}

static void speaker_on(void)
{
	uint8_t tmp;
	/* play the sound using the PC speaker */
	tmp = io_inb(0x61);
	if (tmp != (tmp | 3))
		io_outb(0x61, (tmp | 3));
}

static void speaker_off(void)
{
	uint8_t tmp = (io_inb(0x61) & 0xFC);
	io_outb(0x61, tmp);
}

static void ubeep(uint32_t freq, uint32_t time)
{
	speaker_phase(freq);
	speaker_on();
	for (volatile int i = 0; i < time; i++)
		i = i;
	speaker_off();
}

void init_speaker(void)
{
	uint F[14] = {
		262, 277,
		294, 311,
		330, 330,
		349, 370,
		392, 415,
		440, 466,
		494, 494,
	};

#define I(f, t) ubeep(F[-2+2*f], 20000000*t)
#define B +1
#define D ]/2-F[0]+F[0
#define H /2
	I(1  ,  1  );
	I(2  ,  1  );
	I(3  ,  1  );
	I(1  ,  1  );
	I(1  ,  1  );
	I(2  ,  1  );
	I(3  ,  1  );
	I(1  ,  1  );
	I(3  ,  1  );
	I(4  ,  1  );
	I(5  ,  2  );
	I(3  ,  1  );
	I(4  ,  1  );
	I(5  ,  2  );
	I(5  ,  1 H);
	I(6  ,  1 H);
	I(5  ,  1 H);
	I(4  ,  1 H);
	I(3  ,  1  );
	I(1  ,  1  );
	I(5  ,  1 H);
	I(6  ,  1 H);
	I(5  ,  1 H);
	I(4  ,  1 H);
	I(3  ,  1  );
	I(1  ,  1  );
	I(2  ,  1  );
	I(5 D,  1  );
	I(1  ,  2  );
	I(2  ,  1  );
	I(5 D,  1  );
	I(1  ,  2  );
}
