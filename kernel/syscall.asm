; vim: ft=nasm ai

	GLOBAL __syscall_entry
	GLOBAL __int_return
	EXTERN syscall

[SECTION .text]
[BITS 32]

__syscall_entry:
	sub esp, 8
	push ds
	push es
	pushad
	mov eax, ss
	mov ds, eax
	mov es, eax
	push edx
	push ecx
	push eax
	call syscall
__int_return:
	add esp, 12
	popad
	pop es
	pop ds
	add esp, 8
	iretd
