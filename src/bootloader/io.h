/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/io.h
 *  Description:	File handling module.
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_IO
#define BIGOS_BOOTLOADER_IO

#include <efi.h>
#include <efilib.h>
#include "error.h"

/**
 * @brief	Read from file
 *
 * @param	file - Open file to read from
 * @param	offset - Offset in bytes from the beginning of the file
 * @param	size - Number of bytes to read
 * @param	buffer - Buffer to read to
 *
 * @return	ERR_NONE - success
 *			ERR_FILE_LOAD_FAILURE - failure
 *
 * @note	Buffer is expected to be at least size bytes big
 */
status_t read_file(EFI_FILE_PROTOCOL* file, UINTN offset, UINTN size, void* buffer);

// TODO: balls
status_t read_file_info(EFI_FILE_PROTOCOL* file,EFI_FILE_INFO** file_info);

status_t read_directory_entry(EFI_FILE_PROTOCOL* file, UINTN* size, void** buffer);

#endif // !BIGOS_BOOTLOADER_IO
