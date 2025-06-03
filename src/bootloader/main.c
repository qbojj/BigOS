/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/main.c
 *  Description:	Entry point for GRUBAS bootloader.
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
#include "error.h"
#include "loader.h"
#include "log.h"
#include "exit.h"
#include "ext2.h"
#include "fdt.h"
#include "config.h"
#include "partition.h"

EFI_HANDLE g_image_handle;
EFI_SYSTEM_TABLE* g_system_table;

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table) {
	error_t status;
	g_image_handle = image_handle;
	g_system_table = system_table;
	InitializeLib(image_handle, g_system_table);

	Print(L"GRUB for Academic Systems v0.1\n");
	Print(L"    ______ ____   __  __ ____   ___    _____\n");
  	Print(L"   / ____// __ \\ / / / // __ ) /   |  / ___/\n");
 	Print(L"  / / __ / /_/ // / / // __  |/ /| |  \\__ \\ \n");
	Print(L" / /_/ // _, _// /_/ // /_/ // ___ | ___/ / \n");
	Print(L" \\____//_/ |_| \\____//_____//_/  |_|/____/  \n");
	Print(L"\n");
	log(L"UEFI-boot running...");

	log(L"Initializing loader...");
	status = initialize_loader();
	if(status != ERR_NONE) {
		err(L"Failed to initialize_loader");
		exit();
	}

	log(L"Loading meta config...");
	status = meta_config_load();
	if(status != ERR_NONE) {
		err(L"Failed to load meta config");
		exit();
	}

	log(L"Starting ext2 driver...");
	status = ext2_driver_start();
	if(status != ERR_NONE) {
		err(L"Failed to run ext2 driver");
		exit();
	}

	log(L"Creating partition table...");
	status = partition_table_create();
	if(status != ERR_NONE) {
		err(L"Failed to create partition table");
		exit();
	}

	log(L"Loading config...");
	status = config_load();
	if(status != ERR_NONE) {
		err(L"Failed to load config");
		exit();
	}

	log(L"Getting FDT...");
	status = get_FDT();
	if(status != ERR_NONE) {
		err(L"Failed to get FDT");
		exit();
	}

	log(L"Listing all partitions...");
	partition_table_print();

	exit_boot();
	while(1); // Kernel shouldn't return
}
