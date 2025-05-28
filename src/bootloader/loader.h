/******************************************************************************
 *
 *  File:			bootloader/loader.h
 *  Description:	Structure for bootloader data.
 *  Author:			Maciej Zgierski
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_LOADER
#define BIGOS_BOOTLOADER_LOADER

#include <efi.h>
#include "error.h"

typedef struct {
	EFI_LOADED_IMAGE_PROTOCOL* image;
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* file_system;
	EFI_FILE_PROTOCOL* root;
} loader_t;

error_t initialize_loader(void);

#endif // !BIGOS_BOOTLOADER_LOADER
