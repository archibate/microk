OUTPUT(i386)
ENTRY(_start)

SECTIONS {
	. = 0x10000000;
	PROVIDE(_userbeg = .);

	. = 0xc0000000;
	.vsystab : {
		*(.vsystab)
	}
	.text : {
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
}
