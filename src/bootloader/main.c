#include <efi.h>
#include <efilib.h>

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

	EFI_STATUS error_status;

	// TEST:
	Print(L"Getting FDT...\n");
	void* fdt = getFDT(system_table);
	if(fdt == NULL) {
		Print(L"Failed to get FDT\n");
		return EFI_LOAD_ERROR;
	}

	Print(L"Getting LoadedImageProtocol...\n");
	EFI_LOADED_IMAGE_PROTOCOL* loaded_image;
	EFI_GUID loaded_image_protocol = LOADED_IMAGE_PROTOCOL;
	error_status = system_table->BootServices->HandleProtocol(
		image_handle,
		&loaded_image_protocol,
		(void**)&loaded_image);
	if(EFI_ERROR(error_status)) {
		Print(L"Failed to get UEFI LoadedImageProtocol\n");
		return EFI_LOAD_ERROR;
	}

	Print(L"Getting FileSystemProtocol...\n");
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* file_system;
	EFI_GUID file_system_protocol = SIMPLE_FILE_SYSTEM_PROTOCOL;
	error_status = system_table->BootServices->HandleProtocol(
		loaded_image->DeviceHandle,
		&file_system_protocol,
		(void**)&file_system);
	if(EFI_ERROR(error_status)) {
		Print(L"Failed to get UEFI FileSystemProtocol\n");
		return EFI_LOAD_ERROR;
	}

	// TODO: search the kernel image on ext2 disk instead of boot partition
	Print(L"Opening volume...\n");
	EFI_FILE_PROTOCOL* root;
	error_status = file_system->OpenVolume(file_system, &root);
	if(EFI_ERROR(error_status)) {
		Print(L"Failed to open volume\n");
		return EFI_LOAD_ERROR;
	}

	Print(L"Copying kernel image...\n");
	EFI_FILE_PROTOCOL* kernel;
	error_status = root->Open(root, &kernel, L"kernel.elf", EFI_FILE_MODE_READ, 0);
	if(EFI_ERROR(error_status)) {
		Print(L"Failed to copy kernel image\n");
		return EFI_LOAD_ERROR;
	}

	return EFI_SUCCESS;
}
