/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/io.h
 *  Description:	File handling module.
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_ELF
#define BIGOS_BOOTLOADER_ELF

#include <efi.h>
#include <efilib.h>
#include "error.h"

typedef struct {
	unsigned char ident[16];
	UINT16 type;
	UINT16 machine;
	UINT32 version;
	UINT64 entry;
	UINT64 phoff;
	UINT64 shoff;
	UINT32 flags;
	UINT16 ehsize;
	UINT16 phentsize;
	UINT16 phnum;
	UINT16 shentsize;
	UINT16 shnum;
	UINT16 shstrndx;
} elf64_header_t;

typedef struct {
	UINT32 type;
	UINT32 flags;
	UINT64 offset;
	UINT64 vaddr;
	UINT64 paddr;
	UINT64 filesz;
	UINT64 memsz;
	UINT64 align;
} elf_program_header_t;

typedef struct {
	EFI_FILE_PROTOCOL* file;
	elf64_header_t header;
	elf_program_header_t* program_headers;
	uint64_t image_begin;
	uint64_t image_end;
	uint64_t page_size;

	uint64_t image_pages;
	uint64_t image_addr;
	uint64_t image_entry;
} elf_application_t;

/**
 * @brief	Read from file
 *
 * @param	file - Open file to read from
 * @param	offset - Offset in bytes from the beginning of the file
 * @param	size - Number of bytes to read
 * @param	buffer - Buffer to read to
 *
 * @return	ERR_NONE - success
 *			ERR_FILE_LOAD_FAILURE - failure
 *
 * @note	Buffer is expected to be at least size bytes big
 */
error_t read_file(EFI_FILE_PROTOCOL* file, UINTN offset, UINTN size, void* buffer);

// TODO: balls
error_t read_file_info(EFI_FILE_PROTOCOL* file,EFI_FILE_INFO** file_info);

// TODO: return something more meaningful
EFI_STATUS read_elf_header(EFI_FILE_PROTOCOL* file, elf64_header_t* header);

// TODO: return something more meaningful
UINTN verify_elf_header(elf64_header_t* header);

// TODO: return something more meaningful
EFI_STATUS load_elf(elf_application_t* app);

#endif // !BIGOS_BOOTLOADER_ELF
