#ifndef BIGOS_KERNEL_MEMORY_MANAGEMENT_PHYSICAL_MEMORY_MANAGER
#define BIGOS_KERNEL_MEMORY_MANAGEMENT_PHYSICAL_MEMORY_MANAGER

#include <stdbigos/address.h>
#include <stdbigos/error.h>
#include <stdbigos/memory_types.h>
#include <stdbigos/types.h>
#include <stddef.h>
#include <stdint.h>

/// @ingroup kmm
/// @ingroup pmm
/// @todo implement
static inline void* physical_to_effective([[maybe_unused]] __phys void* addr) {
	return nullptr;
}

static inline memory_region_t physical_memory_region_to_effective(physical_memory_region_t pmr) {
	memory_region_t out = {
	    .addr = physical_to_effective(pmr.addr),
	    .size = pmr.size,
	};
	return out;
}

/// @ingroup kmm
/// @ingroup pmm
typedef enum : u8 {
	FRAME_SIZE_4KiB = 0,
	FRAME_SIZE_8KiB = 1,
	FRAME_SIZE_16KiB = 2,
	FRAME_SIZE_32KiB = 3,
	FRAME_SIZE_64KiB = 4,
	FRAME_SIZE_128KiB = 5,
	FRAME_SIZE_256KiB = 6,
	FRAME_SIZE_512KiB = 7,
	FRAME_SIZE_1MiB = 8,
	FRAME_SIZE_2MiB = 9,
	FRAME_SIZE_1GiB = 18,
} frame_size_t; // NOTE: value of frame_size_t is (4KiB << frame_size)

/// @ingroup kmm
/// @ingroup pmm
u64 phys_mem_get_frame_size_in_bytes(frame_size_t fs);

/**
 *	@ingroup kmm
 *	@ingroup pmm
 *	@brief Initializes physical memory manager
 *
 *	@param pmrs An array of allocatable regions of physical memory
 *	@param pmr_count
 *	@param reserved_areas An array of reserved areas of physical memory of count @p reserved_areas_count
 *	@param reserved_areas_count
 *
 *	@retval ERR_NONE
 *	@retval ERR_REPEATED_INITIALIZATION
 *	@retval ERR_OUT_OF_BOUND Internal memory mangament failed
 *	@retval ERR_BAD_ARG @p pmr_count is zero
 * */
[[gnu::nonnull]]
error_t phys_mem_init(const physical_memory_region_t* pmrs, size_t pmr_count, const memory_area_t* reserved_areas,
                      size_t reserved_areas_count);

/**
 *	@ingroup kmm
 *	@ingroup pmm
 *	@retval ERR_NONE
 *	@retval ERR_OUT_OF_MEMORY The block of specified size was not able to be allocated
 * */
[[gnu::nonnull]]
error_t phys_mem_alloc_frame(frame_size_t frame_size, physical_memory_region_t* regOUT);

/**
 *	@ingroup kmm
 *	@ingroup pmm
 *	@retval ERR_NONE
 *	@retval ERR_NOT_VALID The address being freed was reported as not allocated
 * */
error_t phys_mem_free_frame(physical_memory_region_t reg);

#endif //! BIGOS_KERNEL_MEMORY_MANAGMENT_PHYSICAL_MEMORY_MANAGMENT
