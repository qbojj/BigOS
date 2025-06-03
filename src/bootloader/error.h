/******************************************************************************
 *
 *  Project:		BigOS
 *  File:			bootloader/error.h
 *  Description:	Bootloader error type.
 *
 ******************************************************************************/

#ifndef BIGOS_BOOTLOADER_ERROR
#define BIGOS_BOOTLOADER_ERROR

typedef enum {
	ERR_NONE = 0,
	ERR_FILE_LOAD_FAILURE,
	ERR_FILE_READ_FAILURE,
	ERR_FILE_INFO_READ_FAILURE,
	ERR_LOADER_INIT_FAILURE,
	ERR_PRE_CONFIG_LOAD_FAILURE,
	ERR_CONFIG_LOAD_FAILURE,
	ERR_EXT2_DRIVER_START_FAILURE,
	ERR_PARTITION_TABLE_CREATE_FAILURE,
} error_t;

#endif // !BIGOS_BOOTLOADER_ERROR
