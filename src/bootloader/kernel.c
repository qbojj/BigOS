/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/kernel.c
 *
 ******************************************************************************/

#include "kernel.h"
#include <efi.h>
#include <efidef.h>
#include <efilib.h>
#include "common.h"
#include "exit.h"
#include "log.h"

#define STACK_SIZE (2 * 1024 * 1024)
#define STACK_PAGES (STACK_SIZE / 4096)

void kernel_start(void) {
	START;
	EFI_STATUS status;

	log(L"Creating kernel stack");
	UINT64 stack_addr = 0;
	status = g_system_table->BootServices->AllocatePages(
		AllocateAnyPages,
		EfiLoaderData,
		STACK_PAGES,
		&stack_addr
	);
	if(EFI_ERROR(status)) {
		err(L"Failed to allocate pages");
		END;
		return;
	}

	UINT64 stack_top = stack_addr + STACK_SIZE;

	UINTN map_size = 0;
	EFI_MEMORY_DESCRIPTOR* mem_map = NULL;
	UINTN map_key;
	UINTN desc_size;
	UINT32 desc_version;

	log(L"Exiting UEFI-boot...");
	exit_boot();

	log(L"Creating memory map...");
	status = g_system_table->BootServices->GetMemoryMap(
		&map_size,
		NULL,
		&map_key,
		&desc_size,
		&desc_version
	);
	if(status != EFI_BUFFER_TOO_SMALL) {
		err(L"Failed to get memory map size. BootServices.GetMemoryMap() return code: %u", status);
		END;
		return;
	}

	map_size += 8 * desc_size;

	mem_map = AllocatePool(map_size);
	if(mem_map == NULL) {
		err(L"Failed to allocate memory map");
		END;
		return;
	}

	status = g_system_table->BootServices->GetMemoryMap(
		&map_size,
		mem_map,
		&map_key,
		&desc_size,
		&desc_version
	);
	if(EFI_ERROR(status)) {
		err(L"Failed to create memory map. BootServices.GetMemoryMap() return code: %u", status);
		END;
		return;
	}

	log(L"Starting BigOS...");
	status = g_system_table->BootServices->ExitBootServices(g_image_handle, map_key);
	if(EFI_ERROR(status)) {
		err(L"Failed to exit boot services. BootServices.ExitBootServices() return code: %u", status);
		END;
		return;
	}

	typedef void (*kernel_entry_t)(void*);
	kernel_entry_t entry = (kernel_entry_t)(UINTN)(g_kernel_app.entry_address);

	// Manually jump to kernel entry point
	__asm__ __volatile__ (
		"mv a0, %[arg0]\n"
		"mv sp, %[stack]\n"
		"mv gp, %[stack]\n"
		"jalr zero, %[entry], 0\n"
		:
		:
		[arg0] "r" (g_fdt),
		[stack] "r" (stack_top),
		[entry] "r" (entry)
		: "memory"
	);

	while(1); // Kernel shouldn't return
}
