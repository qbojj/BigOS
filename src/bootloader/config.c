/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/config.c
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

#define META_CONFIG_PATH L"EFI\\BOOT\\conf.meta"

meta_config_t g_meta_config;

void meta_config_unload(void) {
	START;
	log(L"Deleting config data...");
	FreePool(g_meta_config.path);
	END;
}

error_t meta_config_load(void) {
	START;
	EFI_STATUS status;
	error_t read_status;

	EFI_FILE_PROTOCOL* meta_config_file;

	log(L"Opening file...");
	status = g_loader.root->Open(
		g_loader.root,
		&meta_config_file,
		META_CONFIG_PATH,
		EFI_FILE_MODE_READ, 
		EFI_FILE_READ_ONLY
	);
	if(EFI_ERROR(status)) {
		err(L"Failed to open file. Error code: %u", status);
		RETURN(ERR_PRE_CONFIG_LOAD_FAILURE);
	}

	log(L"Getting file info...");
	EFI_FILE_INFO* file_info = NULL;
	UINTN file_info_size = 0;
	EFI_GUID file_info_guid = EFI_FILE_INFO_ID;
	status = meta_config_file->GetInfo(
		meta_config_file,
		&file_info_guid,
		&file_info_size,
		file_info
	);
	if(status != EFI_BUFFER_TOO_SMALL) {
		err(L"Failed to get file info size. Error code: %u", status);
		RETURN(ERR_PRE_CONFIG_LOAD_FAILURE);
	}

	file_info = AllocateZeroPool(file_info_size);
	if(file_info == NULL) {
		err(L"Failed to allocate buffer for file info. Error code: %u", status);
		RETURN(ERR_PRE_CONFIG_LOAD_FAILURE);
	}

	status = meta_config_file->GetInfo(
		meta_config_file,
		&file_info_guid,
		&file_info_size,
		file_info
	);
	if(EFI_ERROR(status)) {
		FreePool(file_info);
		err(L"Failed to get file info. Error code: %u", status);
		RETURN(ERR_PRE_CONFIG_LOAD_FAILURE);
	}

	log(L"Verifying file size...");
	if(file_info->FileSize < sizeof(EFI_GUID)) {
		FreePool(file_info);
		err(L"Invalid pre-config file data. Error code: %u", status);
		RETURN(ERR_PRE_CONFIG_LOAD_FAILURE);
	}

	log(L"Reading file contents...");
	read_status = read_file(meta_config_file, 0, sizeof(EFI_GUID), (void*)&g_meta_config);
	if(read_status != ERR_NONE) {
		FreePool(file_info);
		err(L"Failed to read GUID. Error code: %u", status);
		RETURN(ERR_PRE_CONFIG_LOAD_FAILURE);
	}

	UINTN path_size = file_info->FileSize - sizeof(EFI_GUID);
	CHAR16* path = AllocatePool(path_size + 2);
	if(path == NULL) {
		FreePool(file_info);
		err(L"Failed to allocate buffer for file data. Error code: %u", status);
		RETURN(ERR_PRE_CONFIG_LOAD_FAILURE);
	}

	read_status = read_file(meta_config_file, sizeof(EFI_GUID), path_size, (void*)path);
	if(read_status != ERR_NONE) {
		FreePool(file_info);
		FreePool(path);
		err(L"Failed to read path. Error code: %u", status);
		RETURN(ERR_PRE_CONFIG_LOAD_FAILURE);
	}

	FreePool(file_info);

	path[path_size / 2] = L'\0';
	g_meta_config.path = path;
	g_meta_config.path_size = path_size + 2;
	exit_procedure_register(meta_config_unload);

	log(L"Closing file...");
	meta_config_file->Close(meta_config_file);

	log(L"Meta config loaded:");
	START;
	log(
		L" - Partition GUID: %08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		g_meta_config.partition.Data1, g_meta_config.partition.Data2, g_meta_config.partition.Data3,
		g_meta_config.partition.Data4[0], g_meta_config.partition.Data4[1],
		g_meta_config.partition.Data4[2], g_meta_config.partition.Data4[3],
		g_meta_config.partition.Data4[4], g_meta_config.partition.Data4[5],
		g_meta_config.partition.Data4[6], g_meta_config.partition.Data4[7]
	);
	log(L" - Path: %s", g_meta_config.path);
	END;

	RETURN(ERR_NONE);
}
