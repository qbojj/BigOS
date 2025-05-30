/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/ext2.c
 *
 ******************************************************************************/

#include "ext2.h"

#include <efi.h>
#include <efilib.h>
#include <efiapi.h>
#include <efidef.h>
#include <efidevp.h>
#include "common.h"
#include "error.h"
#include "log.h"

#define EXT2_DRIVER_PATH L"EFI\\BOOT\\ext2.efi"

error_t ext2_driver_start(void) {
	START;
	error_t status;

	log(L"Opening file...");
	EFI_FILE_PROTOCOL* ext2_file;
	status = g_loader.root->Open(
		g_loader.root, 
		&ext2_file, 
		EXT2_DRIVER_PATH, 
		EFI_FILE_MODE_READ, 
		EFI_FILE_READ_ONLY
	);
	if(EFI_ERROR(status)) {
		err(L"Failed to open ext2 driver file. Error code: %u", status);
		RETURN(ERR_EXT2_DRIVER_START_FAILURE);
	}

	log(L"Loading driver image...");
	EFI_DEVICE_PATH_PROTOCOL* ext2_driver_path = FileDevicePath(g_loader.image->DeviceHandle, EXT2_DRIVER_PATH);
	EFI_HANDLE ext2_driver_handle;
	status = g_system_table->BootServices->LoadImage(
		FALSE, 
		g_image_handle, 
		ext2_driver_path, 
		NULL, 
		0, 
		&ext2_driver_handle
	);
	if(EFI_ERROR(status)) {
		err(L"Failed to load ext2 driver image. Error code: %u", status);
		RETURN(ERR_EXT2_DRIVER_START_FAILURE);
	}

	log(L"Starting driver...");
	status = g_system_table->BootServices->StartImage(
		ext2_driver_handle, 
		NULL, 
		NULL
	);
	if(EFI_ERROR(status)) {
		err(L"Failed to start ext2 driver. Error code: %u", status);
		RETURN(ERR_EXT2_DRIVER_START_FAILURE);
	}

	log(L"Closing file...");
	ext2_file->Close(ext2_file);

	RETURN(ERR_NONE);
}
