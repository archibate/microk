// https://github.com/klange/toaruos/blob/cb45fbc0c4b0c335b891db0641dc7ebc89317913/base/usr/include/kernel/elf.h
#pragma once

#include <struct.h>
#include <types.h>

#define ELF32_word  uint32_t
#define ELF32_addr  uint32_t
#define ELF32_off   uint32_t
#define ELF32_half  uint16_t

STRUCT(ELF32_HEADER) {
	unsigned char e_ident[16];
	ELF32_half e_type;
	ELF32_half e_machine;
	ELF32_word e_version;
	ELF32_addr e_entry;
	ELF32_off  e_phoff;
	ELF32_off  e_shoff;
	ELF32_word e_flags;
	ELF32_half e_ehsize;
	ELF32_half e_phentsize;
	ELF32_half e_phnum;
	ELF32_half e_shentsize;
	ELF32_half e_shnum;
	ELF32_half e_shstrndx;
};

// e_ident[0:4]
#define ELF_MAG "\x7FELF"

// e_type:
#define ET_NONE  0
#define ET_REL   1
#define ET_EXEC  2
#define ET_DYN   3
#define ET_CORE  4

// e_machine:
#define EM_NONE  0
#define EM_386   1

// e_version:
#define EV_NONE  0
#define EV_CURR  1


STRUCT(ELF32_PHDR) {
	ELF32_word p_type;
	ELF32_off  p_offset;
	ELF32_addr p_vaddr;
	ELF32_addr p_paddr;
	ELF32_word p_filesz;
	ELF32_word p_memsz;
	ELF32_word p_flags;
	ELF32_word p_align;
};

// p_type:
#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_SHLIB   5
#define PT_PHDR    6


STRUCT(ELF32_SHDR) {
	ELF32_word sh_name;
	ELF32_word sh_type;
	ELF32_word sh_flags;
	ELF32_addr sh_addr;
	ELF32_off  sh_offset;
	ELF32_word sh_size;
	ELF32_word sh_link;
	ELF32_word sh_info;
	ELF32_word sh_addralign;
	ELF32_word sh_entsize;
};

// sh_type:
#define SHT_NONE     0
#define SHT_PROGBITS 1
#define SHT_SYMTAB   2
#define SHT_STRTAB   3
#define SHT_NOBITS   8
#define SHT_REL      9


STRUCT(ELF32_SYM) {
	ELF32_word st_name;
	ELF32_addr st_value;
	ELF32_word st_size;
	unsigned char st_info;
	unsigned char st_other;
	ELF32_half st_shndx;
};

STRUCT(ELF32_REL) {
	ELF32_addr r_offset;
	ELF32_word r_info;
};
