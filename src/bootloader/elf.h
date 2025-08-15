/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/elf.h
 *  Description:	ELF file handling module and standard ELF64 defines.
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_ELF
#define BIGOS_BOOTLOADER_ELF

#include <efi.h>

#include "elf_defines.h"
#include "error.h"

typedef struct {
	Elf64_Shdr* symtab_hdr;
	Elf64_Shdr* rela_dyn_hdr;
	Elf64_Shdr* rela_plt_hdr;
	Elf64_Shdr* strtab_hdr;
	char* strtab;
	Elf64_Sym* symtab;
} elf_relocations_t;

typedef struct {
	EFI_FILE_PROTOCOL* file;
	Elf64_Ehdr header;
	Elf64_Phdr* program_headers;
	Elf64_Shdr* section_headers;
	CHAR8* section_headers_strings;
	elf_relocations_t relocations;

	UINT64 entry_address;

	UINT64 img_begin;
	UINT64 img_end;
	UINT64 base_vaddr;
	UINT64 top_vaddr;
	UINT64 page_count;
	UINT64 size;
	UINT64 physical_base;
} elf_application_t;

[[nodiscard]] status_t elf_load(elf_application_t* app);

#endif // !BIGOS_BOOTLOADER_ELF
