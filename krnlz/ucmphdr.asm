; vim: ft=nasm

KERNADR equ 0x100000

	GLOBAL _start
	EXTERN uncompress

[SECTION .text]
[BITS 32]
_start:
	push KERNADR
	push ebp
	push _kernelz
	call uncompress
	add esp, 12
	jmp KERNADR

[SECTION .rodata]
_kernelz:
	incbin "bin/kernel.bin.z"
