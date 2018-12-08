OUTPUT(i386)
ENTRY(_start)

SECTIONS {
	. = 0xc0000000;
	.text : {
		*(.vsystab)
		*(.text*)
		*(.stext*)
		*(.rdata*)
		*(.rodata*)
	}
	.data : {
		*(.data*)
		*(.sdata*)
	}
	.bss : {
		*(.bss*)
		*(.sbss*)
		*(COMMON)
	}
	.stab : {
		*(.stab)
	}
	.stabstr : {
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
