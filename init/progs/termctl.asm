; vim: ft=nasm ai

	GLOBAL _prog_beg_termctl
	GLOBAL _prog_end_termctl

[SECTION .rodata]
[BITS 32]

_prog_beg_termctl:
	incbin "../isodir/bin/termctl"
_prog_end_termctl:
