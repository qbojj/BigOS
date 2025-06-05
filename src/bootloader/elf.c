#include "elf.h"

#include <efi.h>
#include <efilib.h>
#include "common.h"
#include "error.h"
#include "log.h"
#include "io.h"

#define PAGE_SIZE 4096

static status_t verify_elf_header(elf64_header_t* header) {
	START;
	if( header->ident[0] != 0x7f ||
		header->ident[1] != 'E' ||
		header->ident[2] != 'L' ||
		header->ident[3] != 'F') {
		err(L"No ELF signature: %u %c%c%c", (UINT8)header->ident[0], header->ident[1], header->ident[2], header->ident[3]);
		RETURN(BOOT_ERROR);
	}

	if(header->type != 2) {
		err(L"Unsupported ELF type: %u", header->type);
		RETURN(BOOT_ERROR);
	}

	if(header->ident[4] != 2) {
		err(L"Unsupported ELF class: %u", (UINT8)header->ident[4]);
		RETURN(BOOT_ERROR);
	}

	if(header->phnum == 0) {
		err(L"No program headers");
		RETURN(BOOT_ERROR);
	}
	if(header->phentsize != sizeof(elf_program_header_t)) {
		err(L"Unexpected program header size: %llu", header->phentsize);
		RETURN(BOOT_ERROR);
	}

	RETURN(BOOT_SUCCESS);
}

static status_t read_elf_program_headers(elf_application_t* app) {
	START;
	status_t boot_status;

	app->program_headers = AllocatePool(app->header.phnum * app->header.phentsize);
	if(app->program_headers == NULL) {
		err(L"Failed to allocate memory for program headers");
		RETURN(BOOT_ERROR);
	}

	boot_status = read_file(
		app->file,
		app->header.phoff,
		app->header.phnum * app->header.phentsize,
		app->program_headers
	);
	if(boot_status != BOOT_SUCCESS) {
		err(L"Failed to read file");
		RETURN(BOOT_ERROR);
	}

	RETURN(BOOT_SUCCESS);
}

static status_t elf_initialize_image_info(elf_application_t* app) {
	START;
	app->img_begin = UINT64_MAX;
    app->img_end = 0;
    for (UINTN i = 0; i < app->header.phnum; i++) {
        elf_program_header_t* header = &app->program_headers[i];
        if(header->type != 1)
			continue;
        if(header->vaddr < app->img_begin) {
            app->img_begin = header->vaddr;
        }
        if(header->vaddr + header->memsz > app->img_end) {
            app->img_end = header->vaddr + header->memsz;
        }
    }

	if(app->img_begin == UINT64_MAX) {
		err(L"No Load segments encountered");
		RETURN(BOOT_ERROR);
	}

	UINT64 page_mask = PAGE_SIZE - 1;
	app->base_vaddr = app->img_begin & ~page_mask;
	app->top_vaddr = (app->img_end + page_mask) & ~page_mask;
	app->size = app->top_vaddr - app->base_vaddr;
	app->page_count = app->size / PAGE_SIZE;

	RETURN(BOOT_SUCCESS);
}

static status_t elf_load_segments(elf_application_t* app) {
	START;
	status_t boot_status;

	for(UINTN i = 0; i < app->header.phnum; ++i) {
		elf_program_header_t* prog_header = &app->program_headers[i];
		if(prog_header->type != 1 /* LOAD */) continue;

		UINT64 vaddr = prog_header->vaddr;
		UINT64 filesz = prog_header->filesz;
		UINT64 offset = prog_header->offset;
		UINT64 segment_offset = vaddr - app->base_vaddr;
		UINT64 physical_dest = app->physical_base + segment_offset;

		if(vaddr < app->base_vaddr || (vaddr + filesz) > app->top_vaddr) {
			err(L"Invalid segment memory range");
			RETURN(BOOT_ERROR);
		}

		boot_status = read_file(app->file, offset, filesz, (void*)physical_dest);
		if(boot_status != BOOT_SUCCESS) {
			err(L"Failed to read file");
			RETURN(BOOT_ERROR);
		}
	}

	RETURN(BOOT_SUCCESS);
}

status_t elf_load(elf_application_t* app) {
	START;
	EFI_STATUS status;
	status_t boot_status;

	log(L"Reading ELF header...");
	boot_status = read_file(app->file, 0, sizeof(elf64_header_t), &app->header);
	if(boot_status != BOOT_SUCCESS) {
		err(L"Failed to read ELF header");
		RETURN(BOOT_ERROR);
	}

	log(L"Verifying ELF header...");
	boot_status = verify_elf_header(&app->header);
	if(boot_status != BOOT_SUCCESS) {
		err(L"ELF header verification failed");
		RETURN(BOOT_ERROR);
	}

	log(L"Reading program headers...");
	boot_status = read_elf_program_headers(app);
	if(boot_status != BOOT_SUCCESS) {
		err(L"Failed to read program headers");
		RETURN(BOOT_ERROR);
	}

	log(L"Determining image size...");
	boot_status = elf_initialize_image_info(app);
	if(boot_status != BOOT_SUCCESS) {
		err(L"Failed memory needed");
		RETURN(BOOT_ERROR);
	}

	log(L"Allocating pages...");
	status = g_system_table->BootServices->AllocatePages(
		AllocateAnyPages,
		EfiLoaderData,
		app->page_count,
		&app->physical_base
	);
	if(EFI_ERROR(status)) {
		err(L"Failed to allocate pages");
		RETURN(BOOT_ERROR);
	}
	SetMem((void*)app->physical_base, app->size, 0);

	log(L"Loading ELF segments...");
	boot_status = elf_load_segments(app);
	if(boot_status != BOOT_SUCCESS) {
		g_system_table->BootServices->FreePages(app->physical_base, app->page_count);
		err(L"Failed to load ELF segments");
		RETURN(BOOT_ERROR);
	}

	log(L"Determining entry point address...");
	if(app->header.entry < app->img_begin || app->header.entry >= app->img_end) {
		err(L"Invalid entry point address");
		RETURN(BOOT_ERROR);
	}
	app->entry_address = app->physical_base + (app->header.entry - app->base_vaddr);

	log(L"app info:");
	log(L"entry: %llu", app->entry_address);
	log(L"img_begin: %llu", app->img_begin);
	log(L"img_end: %llu", app->img_end);
	log(L"base_vaddr: %llu", app->base_vaddr);
	log(L"top_vaddr: %llu", app->top_vaddr);
	log(L"physical_base: %llu", app->physical_base);

	RETURN(BOOT_SUCCESS);
}
