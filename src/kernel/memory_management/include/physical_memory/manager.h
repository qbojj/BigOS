#ifndef BIGOS_KERNEL_MEMORY_MANAGEMENT_PHYSICAL_MEMORY_MANAGER
#define BIGOS_KERNEL_MEMORY_MANAGEMENT_PHYSICAL_MEMORY_MANAGER

#include <memory_management/include/common_types.h>
#include <stdbigos/address.h>
#include <stdbigos/error.h>
#include <stdbigos/types.h>
#include <stddef.h>
#include <stdint.h>

#include "stdbigos/array_sizes.h"

typedef __phys void* phys_addr_t;

/// @ingroup kmm
typedef struct {
	size_t size;
	phys_addr_t addr __sized_by(size);
} physical_memory_region_t;

/// @ingroup kmm
static inline memory_area_t phys_mem_reg_to_area(physical_memory_region_t region) {
	memory_area_t area;
	area.addr = (uintptr_t)region.addr;
	area.size = region.size;
	return area;
}

/// @ingroup kmm
static inline physical_memory_region_t area_to_phys_mem_reg(memory_area_t area) {
	physical_memory_region_t region;
	region.addr = (phys_addr_t)area.addr;
	region.size = area.size;
	return region;
}

/// @ingroup kmm
static inline void* physical_to_effective([[maybe_unused]] __phys void* addr) {
	return nullptr;
} // TODO: this is here temporarly

static inline memory_region_t phys_mem_reg_to_reg(physical_memory_region_t pmr) {
	memory_region_t reg = {
	    .size = pmr.size,
	    .addr = physical_to_effective(pmr.addr),
	};
	return reg;
}

/// @ingroup kmm
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
u64 phys_mem_get_frame_size_in_bytes(frame_size_t fs);

/**
 *	@ingroup kmm
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
error_t phys_mem_init([[gnu::nonnull]] const physical_memory_region_t* pmrs, size_t pmr_count,
                      [[gnu::nonnull]] const memory_area_t* reserved_areas, size_t reserved_areas_count);

/**
 *	@ingroup kmm
 *	@retval ERR_NONE
 *	@retval ERR_OUT_OF_MEMORY The block of specified size was not able to be allocated
 * */
error_t phys_mem_alloc_frame(frame_size_t frame_size, [[gnu::nonnull]] phys_addr_t* addrOUT);

/**
 *	@ingroup kmm
 *	@retval ERR_NONE
 *	@retval ERR_NOT_VALID The address being freed was reported as not allocated
 * */
error_t phys_mem_free_frame(phys_addr_t addr);

#endif //! BIGOS_KERNEL_MEMORY_MANAGMENT_PHYSICAL_MEMORY_MANAGMENT
