#include <debug/debug_stdio.h>
#include <dt/dt.h>
#include <stdbigos/buffer.h>
#include <stdbigos/types.h>

void main([[maybe_unused]] u32 hartid, const void* fdt) {
	fdt_t fdt_obj;

	int a = dt_init(fdt, &fdt_obj);
	if (a < 0) {
		DEBUG_PRINTF("DT_INIT FAILED %d", a);
		return;
	}

	u32 main_node = dt_get_node_by_path(&fdt_obj, "/cpus");
	u32 child = dt_get_node_child(&fdt_obj, main_node);
	u32 sibling = dt_get_node_sibling(&fdt_obj, main_node);

	DEBUG_PRINTF("Main node: %u\n", main_node);
	DEBUG_PRINTF("Child: %s\n", dt_get_node_name_ptr(&fdt_obj, child));
	DEBUG_PRINTF("Sibling: %s\n", dt_get_node_name_ptr(&fdt_obj, sibling));

	u32 main_prop = dt_get_prop_by_name(&fdt_obj, main_node, "#address-cells");
	buffer_t buf = dt_get_prop_buffer(&fdt_obj, main_prop);

	const char* prop_name = dt_get_prop_name_ptr(&fdt_obj, main_prop);
	u32 prop_val;
	if (!buffer_read_u32_be(buf, 0, &prop_val)) {
		DEBUG_PRINTF("Bad read from %s\n", prop_name);
		return;
	}

	DEBUG_PRINTF("Main node prop: %s: %u\n", prop_name, prop_val);

	u32 first_prop = dt_get_first_prop(&fdt_obj, main_node);
	u32 next_prop = dt_get_next_prop(&fdt_obj, first_prop);

	DEBUG_PRINTF("First prop: %s\n", dt_get_prop_name_ptr(&fdt_obj, first_prop));
	DEBUG_PRINTF("Next prop: %s\n", dt_get_prop_name_ptr(&fdt_obj, next_prop));
}
