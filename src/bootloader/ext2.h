/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/ext2.h
 *  Description:	Ext2 efi driver load handling.
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_EXT2
#define BIGOS_BOOTLOADER_EXT2

#include <efi.h>
#include "error.h"

/**
 * @brief	Start ext2 driver to make ext2 partitions visible
 *
 * @return	ERR_NONE - success
 *			ERR_EXT2_DRIVER_START_FAILURE - failure
 */
[[nodiscard]] error_t ext2_driver_start(void);

#endif // !BIGOS_BOOTLOADER_EXT2
