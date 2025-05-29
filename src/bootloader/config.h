/******************************************************************************
 *
 *  File:			bootloader/config.h
 *  Description:	Structure for bootloader configuration data.
 *  Author:			Maciej Zgierski
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_CONFIG
#define BIGOS_BOOTLOADER_CONFIG

#include <efi.h>
#include <efidef.h>
#include "error.h"

typedef struct {
	EFI_GUID partition;
	CHAR16* path;
	UINTN path_size;
} meta_config_t;

error_t meta_config_load();

#endif // !BIGOS_BOOTLOADER_CONFIG
