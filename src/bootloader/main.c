/******************************************************************************
 *
 *  File:			bootloader/main.c
 *  Description:	Entry point for GRUBAS bootloader.
 *  Author:			Maciej Zgierski
 *
 *  Note:
 *  It's best to have UEFI documentation on hand when reading this code.
 *
 ******************************************************************************/

#include <efi.h>
#include <efilib.h>
#include <efiapi.h>
#include <efidef.h>
#include <efidevp.h>

#include "common.h"
#include "exit.h"
#include "ext2.h"
#include "fdt.h"
#include "elf.h"
#include "partition.h"

EFI_HANDLE g_image_handle;
EFI_SYSTEM_TABLE* g_system_table;

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table) {
	g_image_handle = image_handle;
	g_system_table = system_table;
	InitializeLib(image_handle, g_system_table);

	Print(L"GRUB for Academic Systems v0.1\n");
	Print(L"\n");
	Print(L"    ______ ____   __  __ ____   ___    _____\n");
	Print(L"   / ____// __ \\ / / / // __ ) /   |  / ___/\n");
	Print(L"  / / __ / /_/ // / / // __  |/ /| |  \\__ \\ \n");
	Print(L" / /_/ // _, _// /_/ // /_/ // ___ | ___/ / \n");
	Print(L" \\____//_/ |_| \\____//_____//_/  |_|/____/  \n");
	Print(L"\n\n");
	Print(L"[ ] UEFI-boot running...\n");

	void* fdt = get_FDT();

	ext2_driver_start(image_handle);

	partition_table_create();

	partition_table_print();

	exit_boot();
	while(1); // Kernel shouldn't return
}
