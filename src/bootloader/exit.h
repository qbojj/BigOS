/******************************************************************************
 *
 *  File:			bootloader/exit.h
 *  Description:	Helper module for ensuring proper boot exit.
 *  Author:			Maciej Zgierski
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_EXIT
#define BIGOS_BOOTLOADER_EXIT

#include <efi.h>
#include <efidef.h>

// Exit EFI loader with error
void exit();

// Prepare for EFI loader exit
void exit_boot();

// Add function to run when exit_boot() is called
void exit_procedure_register(void (*function)(void));

#endif // !BIGOS_BOOTLOADER_EXIT
