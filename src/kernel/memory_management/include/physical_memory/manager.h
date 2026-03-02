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

typedef struct {
	size_t size;
	phys_addr_t addr __sized_by(size);
} physical_memory_region_t;

static inline memory_area_t phys_mem_reg_to_area(physical_memory_region_t region) {
	memory_area_t area;
	area.addr = (uintptr_t)region.addr;
	area.size = region.size;
	return area;
}

static inline physical_memory_region_t area_to_phys_mem_reg(memory_area_t area) {
	physical_memory_region_t region;
	region.addr = (phys_addr_t)area.addr;
	region.size = area.size;
	return region;
}

static inline void* physical_to_effective([[maybe_unused]] __phys void* addr) {
	return nullptr;
} // TODO: this is here temporarly

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
} frame_size_t; // NOTE: value of frame_size_t is 4KiB + 2 ^ (frame_size)

u64 phys_mem_get_frame_size_in_bytes(frame_size_t fs);

error_t phys_mem_init(physical_memory_region_t region, const memory_area_t* reserved_regions, u16 reserved_regs_count);

error_t phys_mem_add_region(physical_memory_region_t reg);

error_t phys_mem_alloc_frame(frame_size_t frame_size, phys_addr_t* addrOUT);

error_t phys_mem_free_frame(phys_addr_t addr);

#endif //! BIGOS_KERNEL_MEMORY_MANAGMENT_PHYSICAL_MEMORY_MANAGMENT
