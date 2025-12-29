#include <debug/debug_stdio.h>
#include <drivers/dt/dt.h>
#include <stdbigos/buffer.h>
#include <stdbigos/types.h>

void main([[maybe_unused]] u32 hartid, const void* fdt) {
	int a;
	if ((a = dt_init(fdt)) < 0) {
		dprintf("DT_INIT FAILED %d", a);
		return;
	}

	dt_node_t root = dt_get_root();
	if (!root) {
		dprintf("GETTING ROOT FAILED");
		return;
	}

	const char* path = "/soc";

	u32 res2 = dt_get_node(fdt, path);

	DEBUG_PRINTF("%u\n", res2);

	char name[64];

	dt_get_node_name(fdt, res2, name, 64);
	DEBUG_PRINTF("%s\n", name);

	u32 res3 = dt_get_prop(fdt, res2, "#address-cells");

	dt_get_prop_name(fdt, res3, name, 64);

	DEBUG_PRINTF("%s\n", name);

	buffer_t buf = dt_get_prop_buffer(fdt, res3);

	u32 dwa;
	(void)buffer_read_u32_be(buf, 0, &dwa);

	DEBUG_PRINTF("Dwa:%u\n", dwa);
}
