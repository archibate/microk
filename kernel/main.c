void main(void)
{
#define init(x) extern void init_##x(void); init_##x();
	init(serial);
	init(ppg);
	init(tmpg);
	init(vpt);
	init(tss);
	init(idt);
	init(pic);
	init(sys);
}
