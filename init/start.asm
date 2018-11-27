; vim: ft=nasm ai

	GLOBAL _start
	EXTERN main

[SECTION .text]
[BITS 32]

_start:
	call main
	jmp $
