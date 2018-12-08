; vim: ft=nasm ai

	GLOBAL _vsyscall_entries
	GLOBAL _exit, open, read, write, close, lseek, execa, fork, brk, sbrk, getpid, getppid, waitpid, getich, openat
	GLOBAL getsafeval, setsafeval, pread, pwrite, readdir

vsystab equ 0xc0000000

[SECTION .text]
[BITS 32]
_vsyscall_entries:
_exit:
	jmp [vsystab + 4*0]
open:
	jmp [vsystab + 4*1]
read:
	jmp [vsystab + 4*2]
write:
	jmp [vsystab + 4*3]
close:
	jmp [vsystab + 4*4]
lseek:
	jmp [vsystab + 4*5]
execa:
	jmp [vsystab + 4*6]
fork:
	jmp [vsystab + 4*7]
brk:
	jmp [vsystab + 4*8]
sbrk:
	jmp [vsystab + 4*9]
getpid:
	jmp [vsystab + 4*10]
getppid:
	jmp [vsystab + 4*11]
waitpid:
	jmp [vsystab + 4*12]
getich:
	jmp [vsystab + 4*13]
openat:
	jmp [vsystab + 4*14]
getsafeval:
	jmp [vsystab + 4*15]
setsafeval:
	jmp [vsystab + 4*16]
pread:
	jmp [vsystab + 4*17]
pwrite:
	jmp [vsystab + 4*18]
readdir:
	jmp [vsystab + 4*19]
