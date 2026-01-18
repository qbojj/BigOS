#ifndef SRC_LIB_DT_COMMON
#define SRC_LIB_DT_COMMON

#include <dt/dt.h>
#include <stdbigos/types.h>

// Offsets are of type u32

// Skip node's tag and name and return first aligned offset after them, >0 if success, 0 if error
u32 dt_skip_node_name(const fdt_t* fdt, dt_node_t node);

// Skip node's properties starting at offset (if they exist) and return first aligned offset after them
// >0 if success, 0 if error
u32 dt_skip_node_properties(const fdt_t* fdt, u32 offset);

// Skip node's tag, name and properties and return first aligned offset after them, >0 if success, 0 if error
u32 dt_skip_node_header(const fdt_t* fdt, dt_node_t node);

// Skip an overarching node's nested nodes starting at the start of the first nested node, >0 if success, 0 if error
u32 dt_skip_nested_nodes(const fdt_t* fdt, dt_node_t nested_node);

#endif // !SRC_LIB_DT_COMMON
