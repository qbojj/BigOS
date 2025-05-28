/******************************************************************************
 *
 *  File:			bootloader/ext2.h
 *  Description:	Interface for loading ext2 efi driver.
 *  Author:			Maciej Zgierski
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_EXT2
#define BIGOS_BOOTLOADER_EXT2

#include <efi.h>
#include "error.h"

[[nodiscard]] error_t ext2_driver_start();

#endif // !BIGOS_BOOTLOADER_EXT2
