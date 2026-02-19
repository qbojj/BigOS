#include "allocator.h"

#include "stdbigos/error.h"

error_t pmallocator_get_header(memory_area_t area, get_next_reserved_region_t enumerator, void* user,
                               memory_area_t* headerOUT) {
	(void)area;
	(void)enumerator;
	(void)user;
	(void)headerOUT;
	return ERR_NOT_IMPLEMENTED;
}

error_t pmallocator_init_region(memory_area_t area, memory_region_t header, get_next_reserved_region_t enumerator,
                                void* user) {
	(void)area;
	(void)header;
	(void)enumerator;
	(void)user;
	return ERR_NOT_IMPLEMENTED;
}

error_t pmallocator_allocate(u8 frame_order, memory_region_t header, memory_area_t* areaOUT) {
	(void)frame_order;
	(void)areaOUT;
	(void)header;
	return ERR_NOT_IMPLEMENTED;
}

error_t pmallocator_free(memory_area_t area, memory_region_t header) {
	(void)area;
	(void)header;
	return ERR_NOT_IMPLEMENTED;
}
