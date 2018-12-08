OUTPUT(i386)
ENTRY(_start)

SECTIONS {
	. = 0x10000000;
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text*)
		*(.stext*)
		*(.rdata*)
		*(.rodata*)
	}
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data*)
		*(.sdata*)
	}
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(.bss*)
		*(.sbss*)
		*(COMMON)
	}
	.stab BLOCK(4K) : ALIGN(4K)
	{
		*(.stab)
	}
	.stabstr BLOCK(4K) : ALIGN(4K)
	{
		*(.stabstr)
	}
	/DISCARD/ : {
		*(.eh_frame*)
		*(.comment*)
		*(.note*)
		*(.GNU*)
		*(.gnu*)
		*(.dyn*)
		*(.interp*)
	}
}
