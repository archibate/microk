; vim: ft=nasm ai

	GLOBAL _prog_beg_hello
	GLOBAL _prog_end_hello

[SECTION .rodata]
[BITS 32]

_prog_beg_hello:
	incbin "../isodir/bin/hello"
_prog_end_hello:
