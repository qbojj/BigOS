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

#define PRE_CONFIG_PATH L"EFI\\BOOT\\pre.conf"

config_t g_config;

void config_unload() {
	START;
	log(L"Deleting config data...");
	FreePool(g_config.path);
	END;
}

error_t config_load() {
	START;
	EFI_STATUS status;
	error_t read_status;

	EFI_FILE_PROTOCOL* pre_config_file;

	log(L"Opening file...");
	status = g_loader.root->Open(
		g_loader.root,
		&pre_config_file,
		PRE_CONFIG_PATH,
		EFI_FILE_MODE_READ, 
		EFI_FILE_READ_ONLY
	);
	if(EFI_ERROR(status)) {
		err(L"Failed to open file. Error code: %u", status);
		RETURN(ERR_PRE_CONFIG_LOAD_FAILURE);
	}

	log(L"Getting file info...");
	EFI_FILE_INFO* pre_config_file_info = NULL;
	UINTN pre_config_file_info_size = 0;
	EFI_GUID file_info_guid = EFI_FILE_INFO_ID;
	status = pre_config_file->GetInfo(
		pre_config_file,
		&file_info_guid,
		&pre_config_file_info_size,
		pre_config_file_info
	);
	if(status != EFI_BUFFER_TOO_SMALL) {
		err(L"Failed to get file info size. Error code: %u", status);
		RETURN(ERR_PRE_CONFIG_LOAD_FAILURE);
	}

	pre_config_file_info = AllocateZeroPool(pre_config_file_info_size);
	if(pre_config_file_info == NULL) {
		err(L"Failed to allocate buffer for file info. Error code: %u", status);
		RETURN(ERR_PRE_CONFIG_LOAD_FAILURE);
	}

	status = pre_config_file->GetInfo(
		pre_config_file,
		&file_info_guid,
		&pre_config_file_info_size,
		pre_config_file_info
	);
	if(EFI_ERROR(status)) {
		FreePool(pre_config_file_info);
		err(L"Failed to get file info. Error code: %u", status);
		RETURN(ERR_PRE_CONFIG_LOAD_FAILURE);
	}

	log(L"Verifying file size...");
	if(pre_config_file_info->FileSize < sizeof(EFI_GUID)) {
		FreePool(pre_config_file_info);
		err(L"Invalid pre-config file data. Error code: %u", status);
		RETURN(ERR_PRE_CONFIG_LOAD_FAILURE);
	}

	log(L"Reading file contents...");
	read_status = read_file(pre_config_file, 0, sizeof(EFI_GUID), (void*)&g_config);
	if(read_status != ERR_NONE) {
		FreePool(pre_config_file_info);
		err(L"Failed to read GUID. Error code: %u", status);
		RETURN(ERR_PRE_CONFIG_LOAD_FAILURE);
	}

	UINTN path_size = pre_config_file_info->FileSize - sizeof(EFI_GUID);
	CHAR16* path = AllocateZeroPool(path_size + 2);
	if(path == NULL) {
		FreePool(pre_config_file_info);
		err(L"Failed to allocate buffer for file data. Error code: %u", status);
		RETURN(ERR_PRE_CONFIG_LOAD_FAILURE);
	}

	read_status = read_file(pre_config_file, sizeof(EFI_GUID), path_size, (void*)&path);
	if(read_status != ERR_NONE) {
		FreePool(pre_config_file_info);
		FreePool(path);
		err(L"Failed to read path. Error code: %u", status);
		RETURN(ERR_PRE_CONFIG_LOAD_FAILURE);
	}

	FreePool(pre_config_file_info);

	path[path_size / 2] = L'\0';
	g_config.path = path;
	g_config.path_size = path_size + 2;
	exit_procedure_register(config_unload);

	log(
		L" - GPT UUID: %08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		g_config.partition.Data1, g_config.partition.Data2, g_config.partition.Data3,
		g_config.partition.Data4[0], g_config.partition.Data4[1],
		g_config.partition.Data4[2], g_config.partition.Data4[3],
		g_config.partition.Data4[4], g_config.partition.Data4[5],
		g_config.partition.Data4[6], g_config.partition.Data4[7]
	);
	log(L"%s", g_config.path);

	RETURN(ERR_NONE);
}
