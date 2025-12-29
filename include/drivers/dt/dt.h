#ifndef DT_H
#define DT_H

#include <stdbigos/buffer.h>
#include <stdbigos/types.h>

// A node is an offset from the start of fdt with FDT_BEGIN_NODE
typedef u32 dt_node_t;

// A property is an offset from the start of fdt with FDT_PROP
typedef u32 dt_prop_t;

// Caches fdt properties and sets root_node for faster future parsing, 0 if success, <0 if error
int dt_init(const void* fdt);

// Reset properties of the currently used fdt
void dt_reset(void);

// Returns the shortened path, while outputting the name with a pointer to it's start and length, nullptr if error
const char* dt_walk(const char* path, const char** name, u32* name_len);

// Get a node in a subtree of the fdt, leave as 0 for global search, >0 if success, 0 if error
dt_node_t dt_get_node_subtree(const void* fdt, dt_node_t node, const char* node_path);

// Get a node globally in the fdt, >0 if success, 0 if error
dt_node_t dt_get_node(const void* fdt, const char* node_path);

// Get a node's name in out, true if success, false if error
bool dt_get_node_name(const void* fdt, dt_node_t node, char* out, u32 out_size);

// Get a node's prop_name property, >0 if success, 0 if error
dt_prop_t dt_get_prop(const void* fdt, dt_node_t node, const char* prop_name);

// Get a prop's name, >0 if success, 0 if error
bool dt_get_prop_name(const void* fdt, dt_node_t prop, char* out, u32 out_size);

// Get a buffer for prop's data
buffer_t dt_get_prop_buffer(const void* fdt, dt_node_t prop);

// Get node's child count, >=0 if success, <0 if error
int dt_node_child_count(const dt_node_t node);

// Get a node's next sibling, >0 if success, 0 if error or last child
dt_node_t dt_get_next_child(const dt_node_t node);

// Returns the node of the root of current fdt, >0 if success, 0 if error
dt_node_t dt_get_root(void);

// Getters for device tree parameters
u32 dt_get_total_size(void);

u32 dt_get_struct_off(void);

u32 dt_get_strings_off(void);

u32 dt_get_struct_size(void);

u32 dt_get_fdt_version(void);

#endif // !DT_H
