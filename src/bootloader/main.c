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
#include <efiapi.h>
#include <efidef.h>
#include <efidevp.h>
#include <efilib.h>

#define EXT2_DRIVER_PATH L"EFI\\BOOT\\ext2.efi"

#define EFI_FDT_GUID \
	{ 0xb1b621d5, 0xf19c, 0x41a5, \
	{ 0x83, 0x0b, 0xd9, 0x15, 0x2c, 0x69, 0xaa, 0xe0 } }

// FDT is created by u-boot and then passed into UEFI system table
void* getFDT(EFI_SYSTEM_TABLE* system_table) {
	EFI_GUID fdt_guid = EFI_FDT_GUID;
	EFI_CONFIGURATION_TABLE* fdt;

	for(UINTN index = 0; index < system_table->NumberOfTableEntries; ++index) {
		fdt = &system_table->ConfigurationTable[index];
		if(CompareGuid(&fdt->VendorGuid, &fdt_guid)) {
			return fdt->VendorTable;
		}
	}

	return NULL;
}

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table) {
	InitializeLib(image_handle, system_table);

	Print(L"GRUB for Academic System v0.1\n");
	Print(L"\n");
	Print(L"    ______ ____   __  __ ____   ___    _____\n");
  	Print(L"   / ____// __ \\ / / / // __ ) /   |  / ___/\n");
 	Print(L"  / / __ / /_/ // / / // __  |/ /| |  \\__ \\ \n");
	Print(L" / /_/ // _, _// /_/ // /_/ // ___ | ___/ / \n");
	Print(L" \\____//_/ |_| \\____//_____//_/  |_|/____/  \n");
	Print(L"\n\n");

	Print(L"[ ] UEFI-boot running...\n");

	EFI_STATUS status;

	// TEST:
	Print(L"[ ] Getting FDT...\n");
	void* fdt = getFDT(system_table);
	if(fdt == NULL) {
		Print(L"[X] Failed to get FDT");
		return EFI_LOAD_ERROR;
	}

	Print(L"[ ] Getting LoadedImageProtocol...\n");
	EFI_LOADED_IMAGE_PROTOCOL* loaded_image;
	EFI_GUID loaded_image_protocol = LOADED_IMAGE_PROTOCOL;
	status = system_table->BootServices->HandleProtocol(
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
	status = system_table->BootServices->HandleProtocol(
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
	status = system_table->BootServices->LoadImage(
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
	status = system_table->BootServices->StartImage(
		ext2_driver_handle, 
		NULL, 
		NULL
	);
	if(EFI_ERROR(status)) {
		Print(L"[X] Failed to start ext2 driver. Error code: %u\n", status);
		return status;
	}

	Print(L"[ ] Locating file system handles...\n");
	EFI_HANDLE* file_systems_table;
	UINTN file_systems_count;
	status = system_table->BootServices->LocateHandleBuffer(
		ByProtocol,
		&file_system_protocol,
		NULL,
		&file_systems_count,
		&file_systems_table
	);
	if(EFI_ERROR(status)) {
		Print(L"[X] Failed to locate file system handles. Error code: %u\n", status);
		return status;
	}

	Print(L"[ ] Located file system handles:\n");
	for(UINTN index = 0; index < file_systems_count; ++index) {
		EFI_HANDLE handle = file_systems_table[index];

		Print(L"\t(%u) - 0x%p:\n", index, handle);

		EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* file_system;
		EFI_GUID file_system_protocol = SIMPLE_FILE_SYSTEM_PROTOCOL;
		status = system_table->BootServices->HandleProtocol(
			handle,
			&file_system_protocol,
			(void**)&file_system
		);
		if(EFI_ERROR(status)) {
			Print(L"\t   [X] Failed to open protocol. Error code: %u\n", status);
			continue;
		}

		EFI_FILE_PROTOCOL* root;
		status = file_system->OpenVolume(file_system, &root);
		if(EFI_ERROR(status)) {
			Print(L"\t   [X] Failed to open root directory. Error code: %u\n", status);
			continue;
		}

		EFI_FILE_SYSTEM_INFO* file_system_info = NULL;
		UINTN file_system_info_size = 0;
		EFI_GUID file_system_info_guid = EFI_FILE_SYSTEM_INFO_ID;
		status = root->GetInfo(
			root, 
			&file_system_info_guid,
			&file_system_info_size,
			file_system_info
		);
		file_system_info = AllocatePool(file_system_info_size);
		if(file_system_info == NULL) {
			Print(L"\t   [X] Failed to allocate %llu bytes for file system info\n", file_system_info_size);
			root->Close(root);
			continue;
		}
		status = root->GetInfo(
			root, 
			&file_system_info_guid,
			&file_system_info_size,
			file_system_info
		);
		if(EFI_ERROR(status)) {
			Print(L"\t   [X] Failed to get file system info. Error code: %u\n", status);
			FreePool(file_system_info);
			root->Close(root);
			continue;
		}

		Print(L"\t   [ ] Volume Label:'%s'\n", file_system_info->VolumeLabel);
		Print(L"\t   [ ] Volume Size:%llu\n", file_system_info->VolumeSize);

		FreePool(file_system_info);
		root->Close(root);

		EFI_DEVICE_PATH_PROTOCOL* device_path;
		EFI_GUID device_path_guid = EFI_DEVICE_PATH_PROTOCOL_GUID;
		status = system_table->BootServices->HandleProtocol(
			handle,
			&device_path_guid,
			(void**)&device_path
		);
		if(EFI_ERROR(status)) {
			Print(L"\t   [X] Failed to get device path. Error code: %u\n", status);
			continue;
		}

		EFI_DEVICE_PATH_PROTOCOL* node = device_path;
		while(!IsDevicePathEnd(node)) {
			if(DevicePathType(node) == MEDIA_DEVICE_PATH && DevicePathSubType(node) == MEDIA_HARDDRIVE_DP) {
				HARDDRIVE_DEVICE_PATH* harddrive_path = (HARDDRIVE_DEVICE_PATH*)node;
				if(harddrive_path->SignatureType == SIGNATURE_TYPE_GUID) {
					EFI_GUID* part_guid = (EFI_GUID*) harddrive_path->Signature;
					Print(L"\t   [ ] GPT UUID\n", part_guid);
					break;
				}
			}
			node = NextDevicePathNode(node);
		}

		Print(L"\t   [ ] Device avaible\n");
	}
	FreePool(file_systems_table);

	// Kernel shouldn't return
	while(1);
}
