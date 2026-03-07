#include <debug/debug_stdio.h>
#include <dt/dt.h>
#include <stdbigos/bitutils.h>
#include <stdbigos/buffer.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>
#include <stddef.h>

#include "dt_common.h"
#include "dt_defines.h"

static buffer_t buffer_strtok(const char** path, const char* sep) {
	if (!path)
		return make_buffer(nullptr, 0);

	const char* p = *path;
	p += strspn(p, sep);

	if (*p == '\0') {
		*path = nullptr;
		return make_buffer(nullptr, 0);
	}

	size_t sz = strcspn(p, sep);

	*path = p + sz;
	return make_buffer(p, sz);
}

dt_node_t dt_get_node_in_subtree_by_path(const fdt_t* fdt, dt_node_t node, const char* node_path) {
	if (!node && !fdt->root_node)
		return 0;
	if (fdt->root_node)
		node = fdt->root_node;

	while (true) {
		buffer_t segment_name = buffer_strtok(&node_path, "/");
		if (!buffer_is_valid(segment_name)) {
			return node;
		}

		node = dt_get_node_child(fdt, node);
		bool found = false;
		while (node) {
			buffer_t child_name = dt_get_node_name(fdt, node);
			if (buffer_equal(child_name, segment_name)) {
				found = true;
				break;
			}

			node = dt_get_node_sibling(fdt, node);
		}

		if (!found)
			return 0;
	}
}

dt_node_t dt_get_node_by_path(const fdt_t* fdt, const char* node_path) {
	return dt_get_node_in_subtree_by_path(fdt, (dt_node_t)0, node_path);
}

dt_node_t dt_get_node_child(const fdt_t* fdt, dt_node_t node) {
	buffer_t fdt_buf = fdt->fdt_buffer;

	u32 curr_offset = dt_skip_node_header(fdt, node);

	if (curr_offset == 0)
		return 0;

	u32 max_offset = fdt->struct_off + fdt->struct_size;

	while (curr_offset < max_offset) {
		u32 tag;
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return 0;

		u32 node_offset = curr_offset;

		curr_offset += sizeof(fdt_token_t);
		switch ((fdt_token_t)tag) {
		case FDT_BEGIN_NODE: return node_offset;

		case FDT_NOP: continue;

		case FDT_END_NODE:
		case FDT_END:      return 0; // No such node found

		default:                // Unknown element type (not defined in v17)
			DEBUG_PRINTF("Invalid FDT structure\n");
			return 0;
		}
	}
	return 0;
}

dt_node_t dt_get_node_sibling(const fdt_t* fdt, dt_node_t node) {
	buffer_t fdt_buf = fdt->fdt_buffer;

	u32 curr_offset = dt_skip_node_header(fdt, node);

	if (curr_offset == 0)
		return 0;

	bool not_nested = false;

	u32 max_offset = fdt->struct_off + fdt->struct_size;

	while (curr_offset < max_offset) {
		u32 tag;
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return 0;

		u32 node_offset = curr_offset;

		curr_offset += sizeof(fdt_token_t);
		switch ((fdt_token_t)tag) {
		case FDT_BEGIN_NODE:
			if (not_nested)
				return node_offset;

			curr_offset = dt_skip_nested_nodes(fdt, node_offset);
			if (curr_offset == 0)
				return 0;
			break;

		case FDT_NOP: continue;

		case FDT_END_NODE: not_nested = true; break;
		case FDT_END:      return 0; // No such node found

		default:                // Unknown element type (not defined in v17)
			DEBUG_PRINTF("Invalid FDT structure\n");
			return 0;
		}
	}

	return 0;
}

buffer_t dt_get_node_name(const fdt_t* fdt, dt_node_t node) {
	buffer_t fdt_buf = fdt->fdt_buffer;

	u32 curr_offset = node;

	u32 tag;

	// Check validity of node
	if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag) || (fdt_token_t)tag != FDT_BEGIN_NODE)
		return make_buffer(nullptr, 0);

	const char* name;
	u64 name_len;
	if (!buffer_read_cstring_len(fdt_buf, curr_offset + sizeof(fdt_token_t), &name, &name_len))
		return make_buffer(nullptr, 0);

	return buffer_sub_buffer(fdt_buf, curr_offset + sizeof(fdt_token_t), name_len);
}

const char* dt_get_node_name_ptr(const fdt_t* fdt, dt_node_t node) {
	buffer_t b = dt_get_node_name(fdt, node);
	return b.data;
}

dt_prop_t dt_get_prop_by_name(const fdt_t* fdt, dt_node_t node, const char* prop_name) {
	if (!node && !fdt->root_node)
		return 0;
	if (fdt->root_node)
		node = fdt->root_node;

	buffer_t name = make_buffer(prop_name, strlen(prop_name));
	dt_prop_t prop = dt_get_first_prop(fdt, node);
	while (prop) {
		buffer_t child_name = dt_get_prop_name(fdt, prop);
		if (buffer_equal(child_name, name)) {
			return prop;
		}

		prop = dt_get_next_prop(fdt, prop);
	}
	return 0;
}

dt_prop_t dt_get_first_prop(const fdt_t* fdt, dt_node_t node) {
	buffer_t fdt_buf = fdt->fdt_buffer;

	u32 curr_offset = dt_skip_node_name(fdt, node);

	if (curr_offset == 0)
		return 0;

	u32 tag;

	u32 max_offset = fdt->struct_off + fdt->struct_size;

	while (curr_offset < max_offset) {
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return 0;

		switch ((fdt_token_t)tag) {
		case FDT_PROP: return curr_offset;

		case FDT_NOP: break;

		// Something else before the first prop
		default: return 0;
		}
	}

	return 0;
}

dt_prop_t dt_get_next_prop(const fdt_t* fdt, dt_prop_t prop) {
	buffer_t fdt_buf = fdt->fdt_buffer;

	u32 curr_offset = prop;

	u32 tag;
	if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
		return 0;

	// Check validity of prop
	if ((fdt_token_t)tag != FDT_PROP)
		return 0;

	u32 p_len;
	if (!buffer_read_u32_be(fdt_buf, curr_offset + sizeof(u32), &p_len))
		return 0;

	curr_offset += 3 * sizeof(u32); // Skip tag, length, name_offset
	curr_offset = align_u32(curr_offset + p_len, sizeof(u32));

	u32 max_offset = fdt->struct_off + fdt->struct_size;

	while (curr_offset < max_offset) {
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return 0;

		switch ((fdt_token_t)tag) {
		case FDT_PROP: return curr_offset;

		case FDT_NOP: break;

		// Something else before the next prop
		default: return 0;
		}
	}

	return 0;
}

buffer_t dt_get_prop_name(const fdt_t* fdt, dt_prop_t prop) {
	buffer_t fdt_buf = fdt->fdt_buffer;

	u32 name_offset;
	if (!buffer_read_u32_be(fdt_buf, prop + 2 * sizeof(fdt_token_t), &name_offset))
		return make_buffer(nullptr, 0);

	const char* name;
	u64 name_len;
	if (!buffer_read_cstring_len(fdt_buf, fdt->strings_off + name_offset, &name, &name_len))
		return make_buffer(nullptr, 0);

	return buffer_sub_buffer(fdt_buf, fdt->strings_off + name_offset, name_len);
}

const char* dt_get_prop_name_ptr(const fdt_t* fdt, dt_prop_t prop) {
	buffer_t buf = dt_get_prop_name(fdt, prop);
	return buf.data;
}

buffer_t dt_get_prop_buffer(const fdt_t* fdt, dt_prop_t prop) {
	buffer_t fdt_buf = fdt->fdt_buffer;

	u32 prop_len;
	if (!buffer_read_u32_be(fdt_buf, prop + sizeof(fdt_token_t), &prop_len))
		return make_buffer(nullptr, 0);

	return buffer_sub_buffer(fdt_buf, prop + 3 * sizeof(fdt_token_t), prop_len);
}
