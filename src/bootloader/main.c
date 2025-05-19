#include <efi.h>
#include <efidef.h>
#include <efilib.h>

#define EXT2_DRIVER_PATH L"EFI\\BOOT\\ext2.efi"

#define EFI_FDT_GUID \
	{ 0xb1b621d5, 0xf19c, 0x41a5, \
	{ 0x83, 0x0b, 0xd9, 0x15, 0x2c, 0x69, 0xaa, 0xe0 } }

// Retrieve FDT (Flattened Device Tree) from EFI System Table
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

	Print(L"BigOS uefi-boot running...\n");

	EFI_STATUS status;

	// TEST:
	Print(L"Getting FDT...\n");
	void* fdt = getFDT(system_table);
	if(fdt == NULL) {
		Print(L"Failed to get FDT");
		return EFI_LOAD_ERROR;
	}

	Print(L"Getting LoadedImageProtocol...\n");
	EFI_LOADED_IMAGE_PROTOCOL* loaded_image;
	EFI_GUID loaded_image_protocol = LOADED_IMAGE_PROTOCOL;
	status = system_table->BootServices->HandleProtocol(
		image_handle,
		&loaded_image_protocol,
		(void**)&loaded_image);
	if(EFI_ERROR(status)) {
		Print(L"Failed to get UEFI LoadedImageProtocol. Error code: %u\n", status);
		return status;
	}

	Print(L"Getting FileSystemProtocol...\n");
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* file_system;
	EFI_GUID file_system_protocol = SIMPLE_FILE_SYSTEM_PROTOCOL;
	status = system_table->BootServices->HandleProtocol(
		loaded_image->DeviceHandle,
		&file_system_protocol,
		(void**)&file_system);
	if(EFI_ERROR(status)) {
		Print(L"Failed to get UEFI FileSystemProtoco. Error code: %u\n", status);
		return status;
	}

	Print(L"Opening volume...\n");
	EFI_FILE_PROTOCOL* boot_part_root;
	status = file_system->OpenVolume(file_system, &boot_part_root);
	if(EFI_ERROR(status)) {
		Print(L"Failed to open volume. Error code: %u\n", status);
		return status;
	}

	Print(L"Opening ext2 driver file...\n");
	EFI_FILE_PROTOCOL* ext2_file;
	status = boot_part_root->Open(boot_part_root, &ext2_file, EXT2_DRIVER_PATH, EFI_FILE_MODE_READ, 0);
	if(EFI_ERROR(status)) {
		Print(L"Failed to open ext2 driver file. Error code: %u\n", status);
		return status;
	}

	Print(L"Loading ext2 driver image...\n");
	EFI_DEVICE_PATH_PROTOCOL* ext2_driver_path = FileDevicePath(loaded_image->DeviceHandle, EXT2_DRIVER_PATH);
	EFI_HANDLE ext2_driver_handle;
	status = system_table->BootServices->LoadImage(FALSE, image_handle, ext2_driver_path, NULL, 0, &ext2_driver_handle);
	if(EFI_ERROR(status)) {
		Print(L"Failed to load ext2 driver image. Error code: %u\n", status);
		return status;
	}

	Print(L"Starting ext2 driver...\n");
	status = system_table->BootServices->StartImage(ext2_driver_handle, NULL, NULL);
	if(EFI_ERROR(status)) {
		Print(L"Failed to start ext2 driver image. Error code: %u\n", status);
		return status;
	}

	return EFI_SUCCESS;
}
