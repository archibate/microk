; vim: ft=nasm ai

	GLOBAL _initrd, _initrd_end
	GLOBAL _vnixrd, _vnixrd_end

[SECTION .rodata]
[BITS 32]

	alignb 4096
_initrd:
	incbin "../isodir/sys/init.bin"
	alignb 4096
_initrd_end:
_vnixrd:
	incbin "../isodir/sys/vnix.bin"
	alignb 4096
_vnixrd_end:
