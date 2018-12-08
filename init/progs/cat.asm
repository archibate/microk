; vim: ft=nasm ai

	GLOBAL _prog_beg_cat
	GLOBAL _prog_end_cat

[SECTION .rodata]
[BITS 32]

_prog_beg_cat:
	incbin "../isodir/bin/cat"
_prog_end_cat:
