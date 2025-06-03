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
#include "partition.h"

#define META_CONFIG_PATH L"EFI\\BOOT\\conf.meta"

meta_config_t g_meta_config;

static INTN guid_compare(EFI_GUID* a, EFI_GUID* b) {
	if(a->Data1 != b->Data1) return 0;
	if(a->Data2 != b->Data2) return 0;
	if(a->Data3 != b->Data3) return 0;
	for(UINTN i = 0; i < 8; ++i) {
		if(a->Data4[i] != b->Data4[i]) return 0;
	}
	return 1;
}

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

	log(L"Reading file info...");
	EFI_FILE_INFO* file_info = NULL;
	read_status = read_file_info(meta_config_file, &file_info);
	if(read_status != ERR_NONE) {
		err(L"Failed to read GUID. Error code: %u", status);
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
	FreePool(file_info);
	CHAR16* path = AllocatePool(path_size + 2);
	if(path == NULL) {
		err(L"Failed to allocate buffer for file data. Error code: %u", status);
		RETURN(ERR_PRE_CONFIG_LOAD_FAILURE);
	}

	read_status = read_file(meta_config_file, sizeof(EFI_GUID), path_size, (void*)path);
	if(read_status != ERR_NONE) {
		FreePool(path);
		err(L"Failed to read path. Error code: %u", status);
		RETURN(ERR_PRE_CONFIG_LOAD_FAILURE);
	}
	path[path_size / 2] = L'\0';

	g_meta_config.path = path;
	g_meta_config.path_size = path_size + 2;
	exit_procedure_register(meta_config_unload);

	log(L"Closing file...");
	meta_config_file->Close(meta_config_file);

	RETURN(ERR_NONE);
}

error_t config_load(void) {
	START;
	EFI_STATUS status;
	error_t read_status;

	partition_t* partition = NULL;
	for(UINTN i = 0; i < g_partition_table_count; ++i) {
		partition_t* current = &g_partition_table[i];
		if(current->guid == NULL) continue;
		// HACK: CompareGuid always returns 0 here for some reason
		if(guid_compare(current->guid, &g_meta_config.partition_guid)) {
			log(L"found");
			partition = current;
			break;
		}
	}
	if(partition == NULL) {
		err(L"Failed to find config partition");
		RETURN(ERR_CONFIG_LOAD_FAILURE);
	}

	log(L"Opening file...");
	log(L"%s", g_meta_config.path);
	EFI_FILE_PROTOCOL* test_file;
	status = partition->root->Open(
		partition->root, 
		&test_file, 
		g_meta_config.path, 
		EFI_FILE_MODE_READ, 
		EFI_FILE_READ_ONLY
	);
	if(EFI_ERROR(status)) {
		err(L"Failed to open file. Error code: %u", status);
		RETURN(ERR_CONFIG_LOAD_FAILURE);
	}

	log(L"Reading file info...");
	EFI_FILE_INFO* test_file_info = NULL;
	read_status = read_file_info(test_file, &test_file_info);
	if(read_status != ERR_NONE) {
		err(L"Failed to read file. Error code: %u", status);
		RETURN(ERR_CONFIG_LOAD_FAILURE);
	}

	CHAR8* text = AllocateZeroPool(test_file_info->FileSize + 1);

	log(L"Reading file contents...");
	read_status = read_file(test_file, 0, test_file_info->FileSize, (void*)text);
	if(read_status != ERR_NONE) {
		FreePool(test_file_info);
		err(L"Failed to read GUID. Error code: %u", status);
		RETURN(ERR_PRE_CONFIG_LOAD_FAILURE);
	}

	for(UINTN i = 0; i < test_file_info->FileSize; ++i) {
		if(text[i] == '\0') break;
		log(L"%c", text[i]);
	}

	RETURN(ERR_NONE);
}
