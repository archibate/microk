; vim: ft=nasm ai

	GLOBAL c4_ipcux

[SECTION .text]
[BITS 32]
c4_ipcux: ; void c4_ipcux(uint to, uint fr, const UT_REGS *wr, UT_REGS *rd)
	push ebp
	mov ebp, esp ; for debug
	push esi
	push edi
	push ebx

	test eax, eax
	jz .noeax
	mov eax, [esp + 28] ; wr
	mov ebx, [eax + 0] ; wr->bx
	mov esi, [eax + 4] ; wr->si
	mov edi, [eax + 8] ; wr->di
	mov edx, [eax + 12] ; wr->dx
	mov ebp, [eax + 16] ; wr->bp
.noeax:
	xor eax, eax ; syscall #0: c4_ipc
	mov ecx, ecx ; ecx = to | (fr << 8)
	mov cl, [esp + 24] ; to
	mov ch, [esp + 28] ; fr
	int 0x80
	mov ecx, [esp + 32] ; rd
	test ecx, ecx
	jz .noecx
	mov [ecx + 0], edx ; rd->bx
	mov [ecx + 4], esi ; rd->si
	mov [ecx + 8], edi ; rd->di
	mov [ecx + 12], ebx ; rd->dx
	mov [ecx + 16], ebx ; rd->bp
.noecx:
	pop ebx
	pop edi
	pop esi
	pop ebp
	ret
