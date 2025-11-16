#include "memory_managment/include/physical_memory/manager.h"

#include <logging/klog.h>
#include <stdbigos/types.h>
#include <memory_managment/common_mem_types.h>
#include "allocator.h"

// Globals

typedef struct {
	physical_memory_region_t allocatable;
	physical_memory_region_t header;
} memory_region_pair_t;

static struct {
	u32 count;
	u32 max_count;
	phys_addr_t data; // Typeof memory_area_t*,
} g_reserved_regions;

static struct {
	u32 count;
	u32 max_count;
	phys_addr_t data; // Typeof memory_region_pair_t*
} g_allocation_regions;

static bool g_is_init = false;

// Internal

memory_area_t* get_reserved_areas_array() {
	return physical_to_effective(g_reserved_regions.data);
}

memory_region_pair_t* get_allocation_regions_array() {
	return physical_to_effective(g_allocation_regions.data);
}

static memory_region_t physical_region_to_effective(physical_memory_region_t reg) {
	const memory_region_t out = {
		.addr = physical_to_effective(reg.addr),
		.size = reg.size,
	};
	return out;
}

static error_t add_area(memory_area_t area, memory_area_t* area_arr, u32* count_ptr, u32 max_count) {
	if (*count_ptr == max_count) {
		KLOG_TRACE_ERROR_RETURN(ERR_OUT_OF_MEMORY);
	}
	area_arr[*count_ptr++] = area;
	return ERR_NONE;
}

static error_t add_region_pair(memory_region_pair_t regp, memory_region_pair_t* regp_arr, u32* count_ptr,
                               u32 max_count) {
	if (*count_ptr == max_count) {
		KLOG_TRACE_ERROR_RETURN(ERR_OUT_OF_MEMORY);
	}
	regp_arr[*count_ptr++] = regp;
	return ERR_NONE;
}

static error_t find_free_subregion(const memory_area_t* res_regs, u32 count, physical_memory_region_t reg, u32 size,
                                   physical_memory_region_t* regOUT) {
	*regOUT = (physical_memory_region_t){0};
	bool found = true;
	phys_addr_t reg_end = reg.addr + reg.size;
	while (reg.addr < reg_end) {
		found = true;
		for (u32 i = 0; i < count; ++i) {
			if (do_memory_areas_overlap(reg, res_regs[i])) {
				found = false;
				break;
			}
		}
		if (found)
			break;
		reg.addr += size;
	}
	if (!found)
		KLOG_TRACE_ERROR_RETURN(ERR_INVALID_MEMORY_REGION);
	*regOUT = reg;
	return ERR_NONE;
}

// Public

// 0x1000 = 2^12 = 4096 = 4kiB

error_t phys_mem_init(physical_memory_region_t prim_reg, const memory_area_t* res_regs, u16 res_regs_count) {
#ifdef __DEBUG__
	if (g_is_init) {
		KLOGLN_WARNING("Attempted to initialize physical memory manager after successful initialization! This is "
		               "permitted, but is probably a mistake!");
	}
#endif
	physical_memory_region_t res_regs_alloc_region = {0};
	error_t err = find_free_subregion(res_regs, res_regs_count, prim_reg, 0x1000, &res_regs_alloc_region);
	if (err)
		KLOG_TRACE_ERROR_RETURN(err);
	g_reserved_regions.count = 0;
	g_reserved_regions.max_count = res_regs_alloc_region.size / sizeof(memory_region_t);
	g_reserved_regions.data = res_regs_alloc_region.addr;
	memory_area_t* reserved_areas_array = get_reserved_areas_array();

	for (u16 i = 0; i < res_regs_count; ++i) {
		err = add_area(memory_area_expand_to_alignment_pow2(res_regs[i], 12), reserved_areas_array,
		                 &g_reserved_regions.count, g_reserved_regions.max_count);
		if (err)
			KLOG_TRACE_ERROR_RETURN(err);
	}
	err = add_area(memory_area_expand_to_alignment_pow2(res_regs_alloc_region, 12), reserved_areas_array,
	                 &g_reserved_regions.count, g_reserved_regions.max_count);
	if (err)
		KLOG_TRACE_ERROR_RETURN(err);

	physical_memory_region_t allocation_regions = {0};
	err = find_free_subregion(reserved_areas_array, g_reserved_regions.count, prim_reg, 0x1000, &allocation_regions);
	if (err)
		KLOG_TRACE_ERROR_RETURN(err);
	err = add_area(memory_area_expand_to_alignment_pow2(allocation_regions, 12), reserved_areas_array,
	                 &g_reserved_regions.count, g_reserved_regions.max_count);
	if (err)
		KLOG_TRACE_ERROR_RETURN(err);
	g_allocation_regions.count = 0;
	g_allocation_regions.max_count = allocation_regions.size / sizeof(memory_region_t);
	g_allocation_regions.data = allocation_regions.addr;

	g_is_init = true;
	return ERR_NONE;
}

error_t phys_mem_add_region(physical_memory_region_t reg) {
	memory_area_t* reserved_areas_array = get_reserved_areas_array();
	size_t header_size = pmallocator_get_header_size(reg);
	physical_memory_region_t header_reg = {0};
	error_t err = find_free_subregion(reserved_areas_array, g_reserved_regions.count, reg, header_size, &header_reg);
	if (err)
		KLOG_TRACE_ERROR_RETURN(err);
	reg = memory_area_shrink_to_alignment_pow2(reg, 12);

	memory_region_pair_t* mem_regs = get_allocation_regions_array();
	memory_region_pair_t regp = {
	    .allocatable = reg,
	    .header = header_reg,
	};

	err = add_area(memory_area_expand_to_alignment_pow2(header_reg, 12), reserved_areas_array,
	                 &g_reserved_regions.count, g_reserved_regions.max_count);
	if (err)
		KLOG_TRACE_ERROR_RETURN(err);

	err = add_region_pair(regp, mem_regs, &g_allocation_regions.count, g_allocation_regions.max_count);
	if (err)
		KLOG_TRACE_ERROR_RETURN(err);

	memory_region_t eff_header_reg = physical_region_to_effective(header_reg);
	err = pmallocator_init_region(reg, eff_header_reg, reserved_areas_array, g_reserved_regions.count);
	if (err)
		KLOG_TRACE_ERROR_RETURN(err);
	// TODO: This should do some cleanup upon failure
	return ERR_NONE;
}

error_t phys_mem_alloc_frame(frame_size_t frame_size, phys_addr_t* addrOUT) {
	phys_addr_t out = 0;

	for (u32 i = 0; i < g_allocation_regions.count; ++i) {
		memory_region_pair_t* alloc_regs_array = get_allocation_regions_array();
		memory_region_t header_reg = physical_region_to_effective(alloc_regs_array[i].header);
		error_t err = pmallocator_allocate(frame_size, &out, alloc_regs_array[i].allocatable, header_reg);
		if (err) {
			KLOG_TRACE_ERROR_RETURN(err);
		}
		*addrOUT = out;
		return ERR_NONE;
	}
	KLOG_TRACE_ERROR_RETURN(ERR_PHYSICAL_MEMORY_FULL);
}

error_t phys_mem_free_frame(phys_addr_t addr) {
	for (u32 i = 0; i < g_allocation_regions.count; ++i) {
		memory_region_pair_t* alloc_regs_array = get_allocation_regions_array();
		const physical_memory_region_t* alloc_reg = &alloc_regs_array[i].allocatable;
		if (alloc_reg->addr >= addr && addr < alloc_reg->addr + alloc_reg->size) {
			memory_region_t header_reg = physical_region_to_effective(alloc_regs_array[i].header);
			error_t err = pmallocator_free(addr, alloc_regs_array[i].allocatable, header_reg);
			if (err)
				KLOG_TRACE_ERROR_RETURN(err);
			return ERR_NONE;
		}
	}
	KLOG_TRACE_ERROR_RETURN(ERR_BAD_ARG);
}
