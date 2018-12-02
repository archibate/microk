; vim: ft=nasm ai

	GLOBAL irq_ents
	EXTERN __int_return
	EXTERN on_hwirq

[SECTION .rodata]

irq_ents:
%include "gen.irqentable.inc"

[SECTION .text]
[BITS 32]

%include "gen.irqentries.inc"

irq_common:
	sub esp, 4 ; reserved
	push dword ds
	push dword es
	pushad

	mov edx, ss
	mov ds, edx
	mov es, edx

	;push dword [esp + 48] ; old eip
	;push ebp ; fake stack frame
	;mov ebp, esp

	mov eax, [esp + 44] ; irq index
	;call [irq_table + eax * 4]
	push eax
	call on_hwirq
	add esp, 4

	;add esp, 8

	jmp __int_return
;__int_return:
	;;mov dword [tss0 + 4], esp
	;;add dword [tss0 + 4], 68
	;;;add dword [tss0 + 4], 60
	;;;test dword [esp + 52], 3
	;;;jz .cont
	;;;add dword [tss0 + 4], 8
;.cont:
	;popad
	;pop dword es
	;pop dword ds
	;add esp, 8 ; skip irq index & reserved one
	;iretd
