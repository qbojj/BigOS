/******************************************************************************
 *
 *  File:			bootloader/log.h
 *  Description:	Helper module for logging within the bootloader.
 *  Author:			Maciej Zgierski
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_LOG
#define BIGOS_BOOTLOADER_LOG

#include <efi.h>
#include <efidef.h>

#define START log_procedure_start()
#define END log_procedure_end()
#define RETURN(x) do { log_procedure_end(); return (x); } while(1)

void log(const CHAR16* message, ...);
void err(const CHAR16* message, ...);

void log_procedure_start();
void log_procedure_end();

#endif // !BIGOS_BOOTLOADER_LOG
