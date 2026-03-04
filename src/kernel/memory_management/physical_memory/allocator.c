#include "allocator.h"

#include "memory_management/include/physical_memory/manager.h"
#include "stdbigos/error.h"

error_t pmallocator_get_header(memory_area_t area, const memory_area_t* reserved_areas, u32 count,
                               memory_area_t* headerOUT) {
	(void)area;
	(void)reserved_areas;
	(void)count;
	(void)headerOUT;
	return ERR_NOT_IMPLEMENTED;
}

error_t pmallocator_init_region(memory_area_t area, memory_region_t header, const memory_area_t* reserved_areas, u32 count) {
	(void)area;
	(void)header;
	(void)reserved_areas;
	(void)count;
	return ERR_NOT_IMPLEMENTED;
}

error_t pmallocator_allocate(u8 frame_order, memory_region_t header, phys_addr_t* addrOUT) {
	(void)frame_order;
	*addrOUT = *addrOUT;
	(void)header;
	return ERR_NOT_IMPLEMENTED;
}

error_t pmallocator_free(phys_addr_t addr, memory_region_t header) {
	(void)addr;
	(void)header;
	return ERR_NOT_IMPLEMENTED;
}
