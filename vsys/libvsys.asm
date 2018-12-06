; vim: ft=nasm ai

	GLOBAL _vsys_calls_entry
	GLOBAL open, read, write, close, exec

vsystab equ 0xc0000000

[SECTION .text]
[BITS 32]
_vsys_calls:
open:
	jmp [vsystab + 4*0]
read:
	jmp [vsystab + 4*1]
write:
	jmp [vsystab + 4*2]
close:
	jmp [vsystab + 4*3]
exec:
	jmp [vsystab + 4*4]
