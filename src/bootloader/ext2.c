/******************************************************************************
 *
 *  File:			bootloader/ext2.h
 *  Description:	Ext2 efi driver loader.
 *  Author:			Maciej Zgierski
 *
 ******************************************************************************/

#include "ext2.h"

#include <efi.h>
#include <efilib.h>
#include <efiapi.h>
#include <efidef.h>
#include <efidevp.h>

#include "common.h"

#define EXT2_DRIVER_PATH L"EFI\\BOOT\\ext2.efi"

EFI_STATUS ext2_driver_start(EFI_HANDLE image_handle) {
	EFI_STATUS status;

	Print(L"[ ] Getting LoadedImageProtocol...\n");
	EFI_LOADED_IMAGE_PROTOCOL* loaded_image;
	EFI_GUID loaded_image_protocol = LOADED_IMAGE_PROTOCOL;
	status = g_system_table->BootServices->HandleProtocol(
		image_handle, 
		&loaded_image_protocol, 
		(void**)&loaded_image
	);
	if(EFI_ERROR(status)) {
		Print(L"[X] Failed to get UEFI LoadedImageProtocol. Error code: %u\n", status);
		return status;
	}

	Print(L"[ ] Getting FileSystemProtocol...\n");
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* file_system;
	EFI_GUID file_system_protocol = SIMPLE_FILE_SYSTEM_PROTOCOL;
	status = g_system_table->BootServices->HandleProtocol(
		loaded_image->DeviceHandle,
		&file_system_protocol, 
		(void**)&file_system
	);
	if(EFI_ERROR(status)) {
		Print(L"[X] Failed to get UEFI FileSystemProtocol. Error code: %u\n", status);
		return status;
	}

	Print(L"[ ] Opening boot volume...\n");
	EFI_FILE_PROTOCOL* boot_part_root;
	status = file_system->OpenVolume(
		file_system, 
		&boot_part_root
	);
	if(EFI_ERROR(status)) {
		Print(L"[X] Failed to open volume. Error code: %u\n", status);
		return status;
	}

	Print(L"[ ] Opening ext2 driver file...\n");
	EFI_FILE_PROTOCOL* ext2_file;
	status = boot_part_root->Open(
		boot_part_root, 
		&ext2_file, 
		EXT2_DRIVER_PATH, 
		EFI_FILE_MODE_READ, 
		0
	);
	if(EFI_ERROR(status)) {
		Print(L"[X] Failed to open ext2 driver file. Error code: %u\n", status);
		return status;
	}

	Print(L"[ ] Loading ext2 driver image...\n");
	EFI_DEVICE_PATH_PROTOCOL* ext2_driver_path = FileDevicePath(loaded_image->DeviceHandle, EXT2_DRIVER_PATH);
	EFI_HANDLE ext2_driver_handle;
	status = g_system_table->BootServices->LoadImage(
		FALSE, 
		image_handle, 
		ext2_driver_path, 
		NULL, 
		0, 
		&ext2_driver_handle
	);
	if(EFI_ERROR(status)) {
		Print(L"[X] Failed to load ext2 driver image. Error code: %u\n", status);
		return status;
	}

	Print(L"[ ] Starting ext2 driver...\n");
	status = g_system_table->BootServices->StartImage(
		ext2_driver_handle, 
		NULL, 
		NULL
	);
	if(EFI_ERROR(status)) {
		Print(L"[X] Failed to start ext2 driver. Error code: %u\n", status);
		return status;
	}

	return EFI_SUCCESS;
}
