; vim: ft=nasm ai

	GLOBAL l4_sysux

[SECTION .text]
[BITS 32]
l4_sysux: ; uint l4_sysux(uint ax, uint cx, const UT_REGS *wr, UT_REGS *rd)
	push ebp
	mov ebp, esp ; for debug
	push esi
	push edi
	push ebx

	mov eax, [esp + 28] ; wr
	test eax, eax
	jz .noeax
	mov ebx, [eax + 0] ; wr->bx
	mov esi, [eax + 4] ; wr->si
	mov edi, [eax + 8] ; wr->di
	mov edx, [eax + 12] ; wr->dx
	mov ebp, [eax + 16] ; wr->bp
.noeax:
	mov eax, [esp + 20] ; ax
	mov ecx, [esp + 24] ; cx
	int 0x80
	mov ecx, [esp + 32] ; rd
	test ecx, ecx
	jz .noecx
	mov [ecx + 0], ebx ; rd->bx
	mov [ecx + 4], esi ; rd->si
	mov [ecx + 8], edi ; rd->di
	mov [ecx + 12], edx ; rd->dx
	mov [ecx + 16], ebp ; rd->bp
.noecx:
	pop ebx
	pop edi
	pop esi
	pop ebp
	ret
