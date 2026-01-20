#ifndef _DT_
#define _DT_

// TODO: Change return values of functions that return e.g. no next properties and errors as the same

#include <stdbigos/buffer.h>
#include <stdbigos/types.h>

// A node is an offset from the start of fdt with FDT_BEGIN_NODE
typedef u32 dt_node_t;

// A property is an offset from the start of fdt with FDT_PROP
typedef u32 dt_prop_t;

typedef struct fdt_t {
	buffer_t fdt_buffer;
	dt_node_t root_node;
	u32 total_size;
	u32 struct_off;
	u32 strings_off;
	u32 struct_size;
	u32 fdt_version;
} fdt_t;

// Read the header at fdt and set fields of obj, 0 if success, <0 if error
// If an error occurs, fdt_buffer of obj is set to an invalid buffer
int dt_init(const void* fdt, fdt_t* obj);

// Reset properties of obj
void dt_reset(fdt_t* obj);

// Get a node in a subtree of fdt, leave as 0 for global search, >0 if success, 0 if error
dt_node_t dt_get_node_in_subtree_by_path(const fdt_t* fdt, dt_node_t node, const char* node_path);

// Get a node globally in fdt, >0 if success, 0 if error
dt_node_t dt_get_node_by_path(const fdt_t* fdt, const char* node_path);

// Get a node's first child in fdt, 0 if no children exist or error, >0 if success
dt_node_t dt_get_node_child(const fdt_t* fdt, dt_node_t node);

// Get a node's next sibling in fdt, 0 if no siblings exist or error, >0 if success
dt_node_t dt_get_node_sibling(const fdt_t* fdt, dt_node_t node);

// Get a node's name as a buffer in fdt, valid buffer if success, invalid buffer if error
buffer_t dt_get_node_name(const fdt_t* fdt, dt_node_t node);

// Get a node's prop_name property in fdt, >0 if success, 0 if error
dt_prop_t dt_get_prop_by_name(const fdt_t* fdt, dt_node_t node, const char* prop_name);

// Get a node's first property in fdt, 0 if no properties exist or error, >0 if success
dt_prop_t dt_get_first_prop(const fdt_t* fdt, dt_node_t node);

// Get a node's next property after prop in fdt, 0 if no next properties exist or error
// >0 if success
dt_prop_t dt_get_next_prop(const fdt_t* fdt, dt_prop_t prop);

// Get a prop's name as a buffer in fdt, valid buffer if success, invalid buffer if error
buffer_t dt_get_prop_name(const fdt_t* fdt, dt_prop_t prop);

// Get a buffer for prop's data
buffer_t dt_get_prop_buffer(const fdt_t* fdt, dt_prop_t prop);

#endif // !_DT_
