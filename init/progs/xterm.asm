; vim: ft=nasm ai

	GLOBAL _prog_beg_xterm
	GLOBAL _prog_end_xterm

[SECTION .rodata]
[BITS 32]

_prog_beg_xterm:
	incbin "../isodir/sys/xterm"
_prog_end_xterm:
