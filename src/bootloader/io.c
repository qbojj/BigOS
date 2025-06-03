/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/io.c
 *
 ******************************************************************************/

#include "io.h"

#include <efi.h>
#include <efilib.h>
#include <efidef.h>
#include "common.h"
#include "log.h"
#include "error.h"

error_t read_file(EFI_FILE_PROTOCOL* file, UINTN offset, UINTN size, void* buffer) {
	START;
	EFI_STATUS status;
	unsigned char* buf = buffer;

	log(L"Set read position to %llu...", offset);
	status = file->SetPosition(file, offset);
	if(EFI_ERROR(status)) {
		err(L"Failed to set read offset. Error code: %u", status);
		RETURN(ERR_FILE_READ_FAILURE);
	}

	log(L"Reading %llu bytes...", size);
	for(UINT64 read = 0; read < size;) {
		UINT64 remains = size - read;
		status = file->Read(file, &remains, (void*)(buf + read));
		if(EFI_ERROR(status))
			RETURN(ERR_FILE_READ_FAILURE);
		read += remains;
	}

	RETURN(ERR_NONE);
}

error_t read_file_info(EFI_FILE_PROTOCOL* file,EFI_FILE_INFO** file_info) {
	START;
	EFI_STATUS status;
	UINTN size = 0;

	EFI_GUID file_info_guid = EFI_FILE_INFO_ID;
	status = file->GetInfo(
		file,
		&file_info_guid,
		&size,
		*file_info
	);
	if(status != EFI_BUFFER_TOO_SMALL) {
		err(L"Failed to get file info size. Error code: %u", status);
		RETURN(ERR_FILE_INFO_READ_FAILURE);
	}

	*file_info = AllocateZeroPool(size);
	if(*file_info == NULL) {
		err(L"Failed to allocate buffer for file info. Error code: %u", status);
		RETURN(ERR_FILE_INFO_READ_FAILURE);
	}

	status = file->GetInfo(
		file,
		&file_info_guid,
		&size,
		*file_info
	);
	if(EFI_ERROR(status)) {
		FreePool(*file_info);
		err(L"Failed to get file info. Error code: %u", status);
		RETURN(ERR_FILE_INFO_READ_FAILURE);
	}

	RETURN(ERR_NONE);
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
