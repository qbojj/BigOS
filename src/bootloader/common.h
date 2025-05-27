/******************************************************************************
 *
 *  File:			bootloader/elf.h
 *  Description:	Header for commonly used values/functions.
 *  Author:			Maciej Zgierski
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_COMMON
#define BIGOS_BOOTLOADER_COMMON

#include <efi.h>

#include "partition.h"

extern EFI_SYSTEM_TABLE* g_system_table;
extern partition_t* g_partition_table;
extern UINTN g_partition_table_count;

#endif // !BIGOS_BOOTLOADER_COMMON
