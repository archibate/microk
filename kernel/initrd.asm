; vim: ft=nasm ai

	GLOBAL _initrd, _initrd_end
	GLOBAL _vsysrd, _vsysrd_end

[SECTION .rodata]
[BITS 32]

	alignb 4096
_initrd:
	incbin "../isodir/sys/init.bin"
	alignb 4096
_initrd_end:
_vsysrd:
	incbin "../isodir/sys/vsys.bin"
	alignb 4096
_vsysrd_end:
