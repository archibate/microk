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
	sub esp, 4072
	push edx
	push ecx
	push eax
	mov eax, ss
	mov ds, eax
	mov es, eax
	call syscall
	add esp, 12
__int_return:
	add esp, 4072
	;mov [esp + 28], eax
	popad
	pop es
	pop ds
	add esp, 8
	iretd
