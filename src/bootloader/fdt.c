/******************************************************************************
 *
 *  File:			bootloader/fdt.c
 *  Author:			Maciej Zgierski
 *
 ******************************************************************************/

#include "fdt.h"

#include <efierr.h>
#include <efilib.h>
#include "common.h"
#include "exit.h"

#define EFI_FDT_GUID \
	{ 0xb1b621d5, 0xf19c, 0x41a5, \
	{ 0x83, 0x0b, 0xd9, 0x15, 0x2c, 0x69, 0xaa, 0xe0 } }

// FDT is created by u-boot and then passed into UEFI system table
void* get_FDT() {
	Print(L"[ ] Getting FDT...\n");
	EFI_GUID fdt_guid = EFI_FDT_GUID;
	EFI_CONFIGURATION_TABLE* fdt;

	for(UINTN index = 0; index < g_system_table->NumberOfTableEntries; ++index) {
		fdt = &g_system_table->ConfigurationTable[index];
		if(CompareGuid(&fdt->VendorGuid, &fdt_guid)) {
			return fdt->VendorTable;
		}
	}

	Print(L"[X] Failed to get FDT");
	exit(EFI_LOAD_ERROR);
	return NULL;
}
