// https://github.com/klange/toaruos/blob/cb45fbc0c4b0c335b891db0641dc7ebc89317913/linker/linker.c
#include "unix.h"
#include <linker/elf.h>
#include <memory.h>
#include <string.h>
#include <errno.h>
#include <numtools.h>
//#include <l4/l4api.h>//l4_print
#include <alloca.h>

static void __attribute__((noreturn)) lunch(void *entry, const char *argl, size_t arglen)
{

	ulong *sp = (ulong*)STACK_END;
	memcpy(sp -= arglen, argl, arglen);
	*--sp = (ulong)entry;
	asm volatile ("mov %0, %%esp\nret\n" :: "r" (sp));
	__builtin_unreachable();
}

static int load_elf(int fd, void **pentry)
{
	ELF32_HEADER elf;
	lseek(fd, 0, SEEK_SET);
	int ret = read(fd, &elf, sizeof(elf));
	if (ret < 0)
		return ret;
	if (memcmp(elf.e_ident, ELF_MAGSTR, 4))
		return -ENOEXEC;
	if (elf.e_type != ET_EXEC)
		return -ENOEXEC;
	if (elf.e_machine != EM_386)
		return -ENOEXEC;

	ulong beg_addr = -1L, end_addr = 0;

	for (int i = 0; i < elf.e_phnum; i++) {
		ELF32_PHDR phdr;
		lseek(fd, elf.e_phoff + elf.e_phentsize * i, SEEK_SET);
		read(fd, &phdr, MAX(sizeof(phdr), elf.e_phentsize));

#if 0 // {{{
		l4_print(phdr.p_type);
		l4_print(phdr.p_vaddr);
		l4_puts("!");
#endif // }}}
		if (phdr.p_type == PT_LOAD) {
#if 0 // {{{
		l4_print(phdr.p_offset);
		l4_print(phdr.p_filesz);
		l4_print(phdr.p_memsz);
		l4_print(phdr.p_vaddr);
		l4_print(elf.e_entry);
#endif // }}}
			beg_addr = MIN(beg_addr, phdr.p_vaddr);
			end_addr = MAX(end_addr, phdr.p_vaddr + phdr.p_memsz);

			lseek(fd, phdr.p_offset, SEEK_SET);
			read(fd, (void*)phdr.p_paddr, phdr.p_filesz);
#if 0 // {{{
			l4_print(phdr.p_paddr);
			l4_print(phdr.p_filesz);
			l4_print(ret);
			l4_print(*(int*)0x10003000);
#endif // }}}

			memset( (void*)(phdr.p_paddr + phdr.p_filesz), 0,
					phdr.p_memsz - phdr.p_filesz);
		}
	}

	*pentry = (void*)elf.e_entry;
	return 0;
}

int execa(const char *path, const char *_argl)
{
	int i = open(path, O_RDONLY | O_EXEC);
	if (i < 0)
		return i;
	size_t arglen = strlen(_argl) + 1;
	char *argl = alloca(arglen);
	memcpy(argl, _argl, arglen);

	void *entry;
	int ret = load_elf(i, &entry);

	if (ret >= 0)
		lunch(entry, argl, arglen);

	close(i);
	return ret;
}
