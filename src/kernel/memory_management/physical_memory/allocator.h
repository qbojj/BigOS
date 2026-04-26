#ifndef BIGOS_KERNEL_MEMORY_MANAGEMENT_PHYSICAL_MEMORY_ALLOCATOR
#define BIGOS_KERNEL_MEMORY_MANAGEMENT_PHYSICAL_MEMORY_ALLOCATOR

#include <stdbigos/error.h>
#include <stdbigos/memory_types.h>
#include <stddef.h>

#include "stdbigos/types.h"

/**
 *
 *	@retval true @p areaOUT is valid
 *	@retval false no more reserved regions
 * */
typedef bool (*get_next_reserved_region_t)(void* user, memory_area_t* areaOUT);

/**
 *	@ingroup kmm
 *	@ingroup palloc
 *
 *	@brief Looks for an area of suitable size and alignmnet to store all metadata about the @p area that the allocator
 * needs
 *
 *	@param area The phiscal memory area to allocate from. Will be aligned to at least 4KiB boundry
 *	@param iterator A pointer to a function that returns reserved regions. Should be nullptr if there are no reserved
 *					regions.
 *
 *	@retval ERR_NONE Success
 *	@retval ERR_OUT_OF_MEMORY Failed to find enough space for the header reagion
 * */
[[gnu::nonnull(3)]]
error_t pmallocator_get_header(memory_area_t area, get_next_reserved_region_t enumerator, void* user,
                               memory_area_t* headerOUT);

/**
 *	@ingroup kmm
 *	@ingroup palloc
 *
 *	@param area The phiscal memory area to allocate from. Will be aligned to at least 4KiB boundry
 *	@param header A memory region of size at least `pmallocator_get_header_size(@p area)` aligned to 4KiB boundry
 *	@param iterator A pointer to a function that returns reserved regions. Should be nullptr if there are no reserved
 *					regions.
 *
 *	@retval ERR_NONE Success
 *
 *	@note The header region overlaps with area, it must be marked and not be allocated from. area.
 *	@note All pointers will break upon change of address space, because this is initialized before and will be used
 *		  after the change, no pointers can be stored inside the `header` region.
 * */
error_t pmallocator_init_region(memory_area_t area, memory_region_t header, get_next_reserved_region_t enumerator,
                                void* user);

/**
 *	@ingroup kmm
 *	@ingroup palloc
 *
 *	@param frame_order The frame size gives as `(1 << frame_order)`
 *	@param addrOUT Pointer the the variable to which the return address will be written to.
 *	The return address will be aligned to the frame size
 *	@param header
 *
 *	@retval ERR_NONE Success
 *	@retval ERR_NOT_VALID The requested frame size is not supported by the allocator
 *	@retval ERR_PHYSICAL_MEMORY_FULL Not enough memory in the area represented by @p header to allocate a frame of
 * desired size
 *
 *	@note 4KiB frame (or @p frame_order = 12) must be a valid frame size.
 * */
[[gnu::nonnull(3)]]
error_t pmallocator_allocate(u8 frame_order, memory_region_t header, memory_area_t* areaOUT);

/**
 *	@ingroup kmm
 *	@ingroup palloc
 *
 *	@param addr
 *	@param header
 *
 *	@retval ERR_NONE Success
 *	@retval ERR_NOT_VALID The area represented by @p addr is already free.
 * */
error_t pmallocator_free(memory_area_t area, memory_region_t header);

#endif
