; vim: ft=nasm ai

	GLOBAL _prog_beg_shell
	GLOBAL _prog_end_shell

[SECTION .rodata]
[BITS 32]

_prog_beg_shell:
	incbin "../isodir/bin/shell"
_prog_end_shell:
