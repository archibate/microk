; vim: ft=nasm ai

	GLOBAL __asm_l4_sysux

[SECTION .text]
[BITS 32]
__asm_l4_sysux: ; l4_recv_ret_t __asm_l4_sysux(uint ax, uint cx, const UT_REGS *snd, UT_REGS *rcv)
	push ebp
	;mov ebp, esp ; for debug
	push esi
	push edi
	push ebx

	test edx, edx ; edx: wr
	jz .noedx
	mov esi, [edx + 4] ; wr->si
	mov edi, [edx + 8] ; wr->di
	mov ebp, [edx + 12] ; wr->bp
	mov ebx, [edx + 16] ; wr->bx
	mov edx, [edx + 0] ; wr->dx
.noedx:
	int 0x80  ; eax: ax, ecx: cx
	pop ebx            ; old ebx: rd
	push ecx ; sysret: ecx: l4_recv_ret_t::flags
	test ebx, ebx
	jz .noebx
	mov [ebx + 0], edx ; rd->dx
	mov [ebx + 4], esi ; rd->si
	mov [ebx + 8], edi ; rd->di
	mov [ebx + 12], ebp ; rd->bp
	mov [ebx + 16], ebx ; rd->bx
.noebx:
	pop ecx ; sysret: ecx: l4_recv_ret_t::flags
	pop edi
	pop esi
	pop ebp
	ret
