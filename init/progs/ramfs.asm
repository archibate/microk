; vim: ft=nasm ai

	GLOBAL _prog_beg_ramfs
	GLOBAL _prog_end_ramfs

[SECTION .rodata]
[BITS 32]

_prog_beg_ramfs:
	incbin "../isodir/sys/ramfs"
_prog_end_ramfs:
