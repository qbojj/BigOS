/******************************************************************************
 *
 *  File:			bootloader/elf.c
 *  Author:			Maciej Zgierski
 *
 ******************************************************************************/

#include "elf.h"

#include <efi.h>
#include <efilib.h>
#include <efidef.h>
#include "common.h"
#include "log.h"

EFI_STATUS read_file(EFI_FILE_PROTOCOL* file, UINT64 offset, UINT64 size, void* buffer) {
	EFI_STATUS status;
	unsigned char* buf = buffer;

	status = file->SetPosition(file, offset);
	if(EFI_ERROR(status)) 
		return status;

	for(UINT64 read = 0; read < size;) {
		UINT64 remains = size - read;
		status = file->Read(file, &remains, (void*)(buf + read));
		if(EFI_ERROR(status))
			return status;
		read += remains;
	}

	return EFI_SUCCESS;
}

EFI_STATUS read_elf_header(EFI_FILE_PROTOCOL* file, elf64_header_t* header) {
	return read_file(file, 0, sizeof(elf64_header_t), header);
}

UINTN verify_elf_header(elf64_header_t* header) {
	if( header->ident[0] != 0x7f ||
		header->ident[1] != 'E' ||
		header->ident[2] != 'L' ||
		header->ident[3] != 'F')
		return 1; // No ELF signature

	if(header->type != 2)
		return 2; // Unsupported elf type

	if(header->ident[4] != 2)
		return 3; // Unsupported elf class

	if(header->phnum == 0)
		return 4; // Elf doesn't contain any program headers

	if(header->phentsize != sizeof(elf64_header_t))
		return 5; // Unexpected header size

	return 0;
}

EFI_STATUS load_elf(elf_application_t* app) {
	log(L"Loading ELF file...");

	EFI_STATUS status;

	UINT64 addr;
	UINT64 size = app->page_size + (app->image_end - app->image_begin);
	app->image_pages = size / app->page_size;

	status = g_system_table->BootServices->AllocatePages(
		AllocateAnyPages,
		EfiLoaderData,
		app->image_pages,
		&addr
	);
	if(EFI_ERROR(status)) return status;

	app->image_addr = addr;

	app->image_entry = app->image_addr + app->page_size + app->header.entry - app->image_begin;

	for(UINTN i = 0; i < size; ++i) {
		((char*)addr)[i] = 0;
	}

	for(UINTN i = 0; i < app->header.phnum; ++i) {
		elf_program_header_t* prog_header = &app->program_headers[i];
		if(prog_header->type != 1 /* LOAD */) continue;

		UINT64 prog_header_addr;
		prog_header_addr = app->image_addr + app->page_size + prog_header->vaddr - app->image_begin;
		status = read_file(app->file, prog_header->offset, prog_header->filesz, (void*)prog_header_addr);
		if(EFI_ERROR(status)) {
			// TODO: handle error
			return status;
		}
	}

	return status;
}
