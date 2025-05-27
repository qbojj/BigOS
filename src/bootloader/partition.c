/******************************************************************************
 *
 *  File:			bootloader/partition.c
 *  Author:			Maciej Zgierski
 *
 ******************************************************************************/

#include "partition.h"

#include <efi.h>
#include <efilib.h>

#include "common.h"
#include "exit.h"

partition_t* g_partition_table;
UINTN g_partition_table_count;

static void partition_create(partition_t* partition, EFI_HANDLE handle) {
	EFI_STATUS status;

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* file_system;
	EFI_GUID file_system_protocol = SIMPLE_FILE_SYSTEM_PROTOCOL;
	status = g_system_table->BootServices->HandleProtocol(
		handle,
		&file_system_protocol,
		(void**)&file_system
	);
	if(EFI_ERROR(status)) return;

	EFI_FILE_PROTOCOL* root;
	status = file_system->OpenVolume(file_system, &root);
	if(EFI_ERROR(status)) return;

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
		root->Close(root);
		return;
	}

	status = root->GetInfo(
		root, 
		&file_system_info_guid,
		&file_system_info_size,
		file_system_info
	);
	if(EFI_ERROR(status)) {
		FreePool(file_system_info);
		root->Close(root);
		return;
	}

	EFI_DEVICE_PATH_PROTOCOL* device_path;
	EFI_GUID device_path_guid = EFI_DEVICE_PATH_PROTOCOL_GUID;
	status = g_system_table->BootServices->HandleProtocol(
		handle,
		&device_path_guid,
		(void**)&device_path
	);
	if(EFI_ERROR(status)) {
		FreePool(file_system_info);
		root->Close(root);
		return;
	}

	partition->flags = 1;
	partition->file_system = file_system;
	partition->root = root;
	partition->file_system_info = file_system_info;
	partition->device_path = device_path;
}

void partition_table_create() {
	EFI_STATUS status;

	Print(L"[ ] Locating file system handles...\n");
	EFI_HANDLE* file_systems_table;
	UINTN file_systems_count;
	EFI_GUID file_system_protocol = SIMPLE_FILE_SYSTEM_PROTOCOL;
	status = g_system_table->BootServices->LocateHandleBuffer(
		ByProtocol,
		&file_system_protocol,
		NULL,
		&file_systems_count,
		&file_systems_table
	);
	if(EFI_ERROR(status)) {
		Print(L"[X] Failed to locate file system handles. Error code: %u\n", status);
		exit(status);
	}

	g_partition_table_count = file_systems_count;
	g_partition_table = AllocateZeroPool(sizeof(partition_t) * file_systems_count);
	if(EFI_ERROR(status)) {
		Print(L"[X] Failed to allocate memory for partition data. Error code: %u\n", status);
		exit(status);
	}

	Print(L"[ ] Creating partition table...\n");
	for(UINTN i = 0; i < g_partition_table_count; ++i) {
		partition_create(&g_partition_table[i], file_systems_table[i]);
	}

	FreePool(file_systems_table);

	exit_procedure_register(partition_table_free);
}

void partition_table_free() {
	Print(L"[ ] Deleting partition table...\n");
	for(UINTN i = 0; i < g_partition_table_count; ++i) {
		if(g_partition_table[i].flags == 0) continue;
		FreePool(g_partition_table[i].file_system_info);
		g_partition_table[i].root->Close(g_partition_table[i].root);
	}
	FreePool(g_partition_table);
}

void partition_print(partition_t* partition) {
	if(partition->flags == 0) {
		Print(L"\tPartition unavaible\n");
		return;
	}

	Print(L"\tPartition avaible:\n");
	Print(L"\t - Volume Label: '%s'\n", partition->file_system_info->VolumeLabel);
	Print(L"\t - Volume Size: %llu\n", partition->file_system_info->VolumeSize);

	EFI_DEVICE_PATH_PROTOCOL* node = partition->device_path;
	while(!IsDevicePathEnd(node)) {
		if(DevicePathType(node) == MEDIA_DEVICE_PATH && DevicePathSubType(node) == MEDIA_HARDDRIVE_DP) {
			HARDDRIVE_DEVICE_PATH* harddrive_path = (HARDDRIVE_DEVICE_PATH*)node;
			if(harddrive_path->SignatureType == SIGNATURE_TYPE_GUID) {
				EFI_GUID* part_guid = (EFI_GUID*) harddrive_path->Signature;
				Print(L"\t - GPT UUID: %g\n", part_guid);
				return;
			}
		}
		node = NextDevicePathNode(node);
	}
}

void partition_table_print() {
	Print(L"Listing all partitions:\n");
	for(UINTN i = 0; i < g_partition_table_count; ++i) {
		Print(L" (%u)", i);
		partition_print(&g_partition_table[i]);
	}
}
