/******************************************************************************
 *
 *  File:			bootloader/partition.h
 *  Description:	Interface for partition access.
 *  Author:			Maciej Zgierski
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_PARTITION
#define BIGOS_BOOTLOADER_PARTITION

#include <efi.h>
#include "error.h"

typedef struct {
	UINTN flags;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* file_system;
	EFI_FILE_PROTOCOL* root;
	EFI_FILE_SYSTEM_INFO* file_system_info;
	EFI_DEVICE_PATH_PROTOCOL* device_path;
	EFI_GUID* guid;
} partition_t;

[[nodiscard]] error_t partition_table_create();
void partition_table_free();

void partition_print(partition_t* partition);
void partition_table_print();

#endif // !BIGOS_BOOTLOADER_PARTITION
