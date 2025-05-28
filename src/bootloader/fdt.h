/******************************************************************************
 *
 *  File:			bootloader/fdt.h
 *  Description:	Interface for FDT manipulation.
 *  Author:			Maciej Zgierski
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_FDT
#define BIGOS_BOOTLOADER_FDT

#include <efi.h>
#include "error.h"

[[nodiscard]] error_t get_FDT();

#endif // !BIGOS_BOOTLOADER_FDT
