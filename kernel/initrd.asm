; vim: ft=nasm ai

	GLOBAL _initrd, _initrd_end

[SECTION .rodata]
[BITS 32]

	alignb 4096
_initrd:
	incbin "../init/bin/init.bin"
	alignb 4096
_initrd_end:
