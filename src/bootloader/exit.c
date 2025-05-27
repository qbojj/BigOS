/******************************************************************************
 *
 *  File:			bootloader/exit.h
 *  Author:			Maciej Zgierski
 *
 ******************************************************************************/

#include "exit.h"

#include <efilib.h>
#include "common.h"

typedef void (*exit_procedure_t)(void);

static UINTN exit_procedure_count = 0;
static UINTN exit_procedure_buffer_size = 0;
static exit_procedure_t* exit_procedures;

#define BUFFER_CHUNK_SIZE 64

static void exit_procedures_call() {
	for(UINTN i = 0; i < exit_procedure_count; ++i) {
		exit_procedures[i]();
	}
	FreePool(exit_procedures);
}

void exit(EFI_STATUS status) {
	exit_procedures_call();
	g_system_table->BootServices->Exit(g_image_handle, status, 0, NULL);
}

void exit_boot() {
	exit_procedures_call();

	Print(L"[ ] Passing control to kernel...\n");
	Print(L"[ ] Exiting UEFI-boot...\n");
	// TODO: exit boot services
}

void exit_procedure_register(exit_procedure_t function) {
	if(exit_procedure_buffer_size == 0) {
		exit_procedures = AllocatePool(sizeof(exit_procedure_t) * BUFFER_CHUNK_SIZE);
	}

	exit_procedure_count++;

	if(exit_procedure_count > exit_procedure_buffer_size) {
		ReallocatePool(exit_procedures, exit_procedure_buffer_size, exit_procedure_buffer_size * 2);
		exit_procedure_buffer_size += BUFFER_CHUNK_SIZE;
	}

	exit_procedures[exit_procedure_count - 1] = function;
}
