/******************************************************************************
 *
 *  File:			bootloader/error.h
 *  Description:	Error handling module
 *  Author:			Maciej Zgierski
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_ERROR
#define BIGOS_BOOTLOADER_ERROR

typedef enum {
	ERR_NONE = 0,
	ERR_PARTITION_TABLE_CREATE,
	ERR_EXT2_DRIVER_START_FAIL,
} error_t;

#endif // !BIGOS_BOOTLOADER_ERROR
