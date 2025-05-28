/******************************************************************************
 *
 *  File:			bootloader/config.c
 *  Author:			Maciej Zgierski
 *
 ******************************************************************************/

#include "config.h"

#include <efi.h>
#include <efilib.h>
#include <efidef.h>
#include "common.h"
#include "error.h"
#include "exit.h"
#include "log.h"
#include "io.h"

config_t g_config;

void config_unload() {
	FreePool(g_config.path);
}

error_t config_load(const CHAR16* pre_config_file_path) {
	START;
	EFI_STATUS status;

	EFI_FILE_PROTOCOL* pre_config_file;

	status = g_loader.root->Open(
		g_loader.root,
		&pre_config_file,
		(CHAR16*)pre_config_file_path,
		EFI_FILE_MODE_READ,
		EFI_FILE_READ_ONLY
	);
	if(EFI_ERROR(status)) {
		err(L"Failed to open file. Error code: %u", status);
		RETURN(ERR_EXT2_DRIVER_START_FAILURE);
	}

	EFI_FILE_SYSTEM_INFO* pre_config_file_info = NULL;
	UINTN pre_config_file_info_size = 0;
	EFI_GUID file_info_guid = EFI_FILE_INFO_ID;
	g_loader.root->GetInfo(
		g_loader.root,
		&file_info_guid,
		&pre_config_file_info_size,
		pre_config_file_info
	);

	pre_config_file_info = AllocateZeroPool(pre_config_file_info_size);
	if(pre_config_file_info == NULL) {
		err(L"Failed to allocate buffer for file info. Error code: %u", status);
		RETURN(ERR_EXT2_DRIVER_START_FAILURE);
	}

	status = g_loader.root->GetInfo(
		g_loader.root,
		&file_info_guid,
		&pre_config_file_info_size,
		pre_config_file_info
	);
	if(EFI_ERROR(status)) {
		FreePool(pre_config_file_info);
		err(L"Failed to get file info. Error code: %u", status);
		RETURN(ERR_EXT2_DRIVER_START_FAILURE);
	}

	if(pre_config_file_info->Size < sizeof(EFI_GUID)) {
		FreePool(pre_config_file_info);
		err(L"Invalid pre-config file data. Error code: %u", status);
		RETURN(ERR_EXT2_DRIVER_START_FAILURE);
	}

	if(read_file(pre_config_file, 0, sizeof(EFI_GUID), (void*)&g_config) != ERR_NONE) {
		FreePool(pre_config_file_info);
		err(L"Failed to read GUID. Error code: %u", status);
		RETURN(ERR_EXT2_DRIVER_START_FAILURE);
	}

	UINTN config_path_size = pre_config_file_info->Size - sizeof(EFI_GUID);
	CHAR16* config_path = AllocatePool(config_path_size);
	if(config_path == NULL) {
		FreePool(pre_config_file_info);
		err(L"Failed to allocate buffer for file data. Error code: %u", status);
		RETURN(ERR_EXT2_DRIVER_START_FAILURE);
	}

	if(read_file(pre_config_file, sizeof(EFI_GUID), config_path_size, (void*)&config_path) != ERR_NONE) {
		FreePool(pre_config_file_info);
		FreePool(config_path);
		err(L"Failed to read path. Error code: %u", status);
		RETURN(ERR_EXT2_DRIVER_START_FAILURE);
	}

	FreePool(pre_config_file_info);

	g_config.path = config_path;
	g_config.path_size = config_path_size;
	exit_procedure_register(config_unload);

	RETURN(ERR_NONE);
}
