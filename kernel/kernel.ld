OUTPUT(i386)
ENTRY(_start)

SECTIONS {
	PROVIDE(_ramdisk = 0x7c00);

	. = 0x100000;
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
	/DISCARD/ : {
		*(.eh_frame*)
		*(.comment*)
		/**(.note*)
		*(.GNU*)
		*(.gnu*)
		*(.dyn*)
		*(.interp*)*/
	}
}
