#include <debug/debug_stdio.h>
#include <drivers/dt/dt.h>
#include <stdbigos/bitutils.h>
#include <stdbigos/buffer.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>

#include "dt_defines.h"

const char* dt_walk(const char* path, const char** name, u32* name_len) {
	if (!path || path[0] != '/')
		return nullptr;

	const char* p = path;

	/* skip leading slashes */
	while (*p == '/') p++;

	if (*p == '\0')
		return nullptr;

	const char* start = p;
	while (*p && *p != '/') p++;

	*name = start;
	*name_len = (u32)(p - start);

	while (*p == '/') p += 1;

	// If this is the last node, dt_walk will return the null terminator
	if (*p != '\0')
		p -= 1;

	return (*p) ? p : nullptr;
}

dt_node_t dt_get_node_subtree(const void* fdt, dt_node_t node, const char* node_path) {
	if (!node)
		node = dt_get_root();

	buffer_t fdt_buf = make_buffer(fdt, dt_get_total_size());

	u32 curr_offset = node;

	u32 tag;
	if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag) || (fdt_token_t)tag != FDT_BEGIN_NODE)
		return 0;

	curr_offset += sizeof(u32);

	const char* name;
	if (!buffer_read_cstring(fdt_buf, curr_offset, &name))
		return 0;

	// After this point all reads from the buffer should be correct
	u32 name_len = strlen(name) + 1;

	curr_offset = align_u32(curr_offset + name_len, sizeof(u32));

	u32 max_offset = dt_get_struct_off() + dt_get_struct_size();

	u32 props_off = curr_offset;
	while (curr_offset < max_offset) {
		u32 tag;
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return 0;

		if ((fdt_token_t)tag != FDT_PROP)
			break;

		u32 p_len;
		if (!buffer_read_u32_be(fdt_buf, curr_offset + 4, &p_len))
			return 0;

		curr_offset += 12; // Skip tag, length, name_offset
		curr_offset = align_u32(curr_offset + p_len, sizeof(u32));
	}
	u32 props_len = curr_offset - props_off;

	curr_offset = align_u32(props_off + props_len, sizeof(u32));

	const char* next_node;
	u32 node_name_len;
	node_path = dt_walk(node_path, &next_node, &node_name_len);

	while (curr_offset < max_offset) {
		u32 tag;
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return 0;

		u32 node_offset = curr_offset;

		curr_offset += sizeof(fdt_token_t);
		switch ((fdt_token_t)tag) {
		case FDT_BEGIN_NODE:

			const char* name;
			if (!buffer_read_cstring(fdt_buf, curr_offset, &name))
				return 0;

			u32 name_len = strlen(name) + 1;

			curr_offset = align_u32(curr_offset + name_len, sizeof(u32));

			bool wrong_child = false;

			// Check if this is the child we're looking for
			if (name_len - 1 != node_name_len) {
				wrong_child = true;
			}

			size_t i = 0;
			while (i < node_name_len && !wrong_child) {
				if (next_node[i] != name[i])
					wrong_child = true;

				i += 1;
			}

			if (!wrong_child) {
				// If this is the end child, process properties here
				if (node_path == NULL) {
					return node_offset;
				} else {
					return dt_get_node_subtree(fdt, node_offset, node_path);
				}
			} else {
				// Skip this node
				size_t depth = 1;
				// Depth to ignore all nested nodes
				while (curr_offset < max_offset && depth > 0) {
					u32 tag;
					if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
						return 0;

					curr_offset += sizeof(fdt_token_t);

					switch (tag) {
					case FDT_BEGIN_NODE:

						depth += 1;
						const char* new_name;
						if (!buffer_read_cstring(fdt_buf, curr_offset, &new_name))
							return 0;

						u32 new_name_len = strlen(new_name) + 1;

						curr_offset = align_u32(curr_offset + new_name_len, sizeof(u32));

						break;

					case FDT_END_NODE: depth -= 1; break;

					case FDT_PROP:
						curr_offset -= sizeof(fdt_token_t);
						u32 props_off = curr_offset;
						while (curr_offset < max_offset) {
							u32 tag;
							if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
								return 0;

							if ((fdt_token_t)tag != FDT_PROP) {
								break;
							}

							u32 p_len;
							if (!buffer_read_u32_be(fdt_buf, curr_offset + 4, &p_len))
								return 0;

							curr_offset += 12; // Skip tag, length, name_offset
							curr_offset = align_u32(curr_offset + p_len, sizeof(u32));
						}
						u32 props_len = curr_offset - props_off;

						curr_offset = align_u32(props_off + props_len, sizeof(u32));
						break;

					case FDT_NOP: continue;

					case FDT_END:
					default: // Unknown element type (not defined in v17)
						DEBUG_PRINTF("Invalid FDT structure\n");
						return 0;
					}
				}
			}
			break;

		case FDT_NOP: continue;

		case FDT_END_NODE:
		case FDT_END:      return 0; // No such node found

		default:                // Unknown element type (not defined in v17)
			DEBUG_PRINTF("Invalid FDT structure\n");
			return 0;
		}
	}

	DEBUG_PRINTF("Invalid FDT structure\n");
	return 0;
}

dt_node_t dt_get_node(const void* fdt, const char* node_path) {
	return dt_get_node_subtree(fdt, (dt_node_t)0, node_path);
}

bool dt_get_node_name(const void* fdt, dt_node_t node, char* out, u32 out_size) {
	buffer_t fdt_buf = make_buffer(fdt, dt_get_total_size());

	const char* name;
	if (!buffer_read_cstring(fdt_buf, node + sizeof(fdt_token_t), &name))
		return nullptr;

	// Buffer too small
	if (strlen(name) + 1 > out_size)
		return false;

	strcpy(out, name);

	return true;
}

// Function to parse a block of properties starting at props_offset with props_size size in the FDT with fdt
// being a ptr to the start of the flattened device tree blob
// TODO: Change this to work in tandem with the static parse_subtree, maybe change names, take from HERE
/*
buffer_t* parse_props(const buffer_t* fdt_buf, u32 props_offset, u32 props_size, u32 str_offset) {
    u32 curr_offset = props_offset;

    dt_prop_t* head = nullptr;
    dt_prop_t** pp = &head;

    while (curr_offset < props_offset + props_size) {
        u32 tag;
        if (!buffer_read_u32_be(*fdt_buf, curr_offset, &tag))
            return nullptr;

        // Because of the separation of parsing properties and nodes, we don't want to parse non-properties
        if ((fdt_token_t)tag != FDT_PROP)
            break;

        curr_offset += 4;
        u32 len;
        u32 name_offset;

        if (!buffer_read_u32_be(*fdt_buf, curr_offset + 0, &len) ||
            !buffer_read_u32_be(*fdt_buf, curr_offset + 4, &name_offset))
            return nullptr;

        curr_offset += 8;

        const char* name;
        if (!buffer_read_cstring(*fdt_buf, str_offset + name_offset, &name))
            return nullptr;

        dt_prop_t* new_prop = dt_alloc(sizeof(*new_prop));
        if (!new_prop)
            return nullptr;

        *new_prop = (dt_prop_t){
            .name = name,
            .data = buffer_sub_buffer(*fdt_buf, curr_offset, len),
            .next_prop = nullptr,
        };

        if (!buffer_is_valid(new_prop->data))
            return nullptr;

        // At first it sets the head in the right place, then it sets the next_prop of the previous property to
point to
        // the current property
        *pp = new_prop;
        pp = &new_prop->next_prop;

        curr_offset = align_u32(curr_offset + len, sizeof(u32));
    }
    return head;
}
*/
// Function to recursively parse a subtree at the given offset to find node by path and by name
// TODO: change all nullptr returns to invalid buffers, after finding the end correct node find the searched-after
// property and get it's buffer
buffer_t parse_subtree(const void* fdt, dt_node_t node, const char* node_path, const char* prop_name) {
	if (!node)
		node = dt_get_root();

	buffer_t fdt_buf = make_buffer(fdt, dt_get_total_size());

	u32 curr_offset = node;

	u32 tag;
	if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag) || (fdt_token_t)tag != FDT_BEGIN_NODE)
		return make_buffer(nullptr, 0);

	curr_offset += sizeof(u32);

	const char* name;
	if (!buffer_read_cstring(fdt_buf, curr_offset, &name))
		return make_buffer(nullptr, 0);

	// After this point all reads from the buffer should be correct

	u32 name_len = strlen(name) + 1;

	curr_offset = align_u32(curr_offset + name_len, sizeof(u32));

	u32 max_offset = dt_get_struct_off() + dt_get_struct_size();

	u32 props_off = curr_offset;
	while (curr_offset < max_offset) {
		u32 tag;
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return make_buffer(nullptr, 0);

		if ((fdt_token_t)tag != FDT_PROP)
			break;

		u32 p_len;
		if (!buffer_read_u32_be(fdt_buf, curr_offset + 4, &p_len))
			return make_buffer(nullptr, 0);

		curr_offset += 12; // Skip tag, length, name_offset
		curr_offset = align_u32(curr_offset + p_len, sizeof(u32));
	}
	u32 props_len = curr_offset - props_off;

	curr_offset = align_u32(props_off + props_len, sizeof(u32));

	const char* next_node;
	u32 node_name_len;
	node_path = dt_walk(node_path, &next_node, &node_name_len);

	while (curr_offset < max_offset) {
		u32 tag;
		if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
			return make_buffer(nullptr, 0);

		u32 node_offset = curr_offset;

		curr_offset += sizeof(fdt_token_t);
		switch ((fdt_token_t)tag) {
		case FDT_BEGIN_NODE:

			const char* name;
			if (!buffer_read_cstring(fdt_buf, curr_offset, &name))
				return make_buffer(nullptr, 0);

			u32 name_len = strlen(name) + 1;

			curr_offset = align_u32(curr_offset + name_len, sizeof(u32));
			// Check if this is the child we're looking for

			dprintf("name: %s\n", name);

			bool wrong_child = false;

			if (name_len - 1 != node_name_len) {
				wrong_child = true;
			}

			size_t i = 0;
			while (i < node_name_len && !wrong_child) {
				if (next_node[i] != name[i])
					wrong_child = true;

				i += 1;
			}

			if (!wrong_child) {
				// If this is the end child, process properties here
				if (node_path == NULL) {
					return make_buffer(fdt + node_offset, max_offset - node_offset);
				} else {
					return parse_subtree(fdt, node_offset, node_path, prop_name);
				}
			} else {
				// Skip this node
				size_t depth = 1;
				// Depth to ignore all nested nodes
				while (curr_offset < max_offset && depth > 0) {
					u32 tag;
					if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
						return make_buffer(nullptr, 0);

					curr_offset += sizeof(fdt_token_t);

					switch (tag) {
					case FDT_BEGIN_NODE:

						depth += 1;
						const char* new_name;
						if (!buffer_read_cstring(fdt_buf, curr_offset, &new_name))
							return make_buffer(nullptr, 0);

						u32 new_name_len = strlen(new_name) + 1;

						curr_offset = align_u32(curr_offset + new_name_len, sizeof(u32));

						break;

					case FDT_END_NODE: depth -= 1; break;

					case FDT_PROP:
						curr_offset -= sizeof(fdt_token_t);
						u32 props_off = curr_offset;
						while (curr_offset < max_offset) {
							u32 tag;
							if (!buffer_read_u32_be(fdt_buf, curr_offset, &tag))
								return make_buffer(nullptr, 0);

							if ((fdt_token_t)tag != FDT_PROP) {
								break;
							}

							u32 p_len;
							if (!buffer_read_u32_be(fdt_buf, curr_offset + 4, &p_len))
								return make_buffer(nullptr, 0);

							curr_offset += 12; // Skip tag, length, name_offset
							curr_offset = align_u32(curr_offset + p_len, sizeof(u32));
						}
						u32 props_len = curr_offset - props_off;

						curr_offset = align_u32(props_off + props_len, sizeof(u32));
						break;

					case FDT_NOP: continue;

					case FDT_END:
					default: // Unknown element type (not defined in v17)
						DEBUG_PRINTF("Invalid FDT structure\n");
						// continue;
						return make_buffer(nullptr, 0);
					}
				}
			}
			break;

		case FDT_NOP: continue;

		case FDT_END_NODE:
		case FDT_END:      return make_buffer(nullptr, 0); // No such node found

		default:                                      // Unknown element type (not defined in v17)
			DEBUG_PRINTF("Invalid FDT structure\n");
			return make_buffer(nullptr, 0);
		}
	}

	DEBUG_PRINTF("END\n");
	return make_buffer(nullptr, 0);
}
/*
// TODO: Part of this function could be generalized then used by the parser and this without duplication
buffer_t dt_prop_get_immediate(const void* fdt, const char* path, const char* p_name) {
    if (!p_name)
        return make_buffer(nullptr, 0); // invalid buffer

    if (!fdt)
        return make_buffer(nullptr, 0);

    u32 magic = read_be32(fdt);
    if (magic != FDT_MAGIC)
        return make_buffer(nullptr, 0); // invalid buffer

    u32 fdt_size = read_be32((u8*)fdt + FDT_OFF_TOTAL_SIZE);

    buffer_t fdt_buf = make_buffer(fdt, fdt_size);

    if (fdt_buf.size < (FDT_OFF_OFF_DT_STRINGS + 4))
        return make_buffer(nullptr, 0); // invalid buffer

    u32 total_size;
    u32 struct_off;
    u32 strings_off;
    u32 struct_size;
    u32 fdt_version;
    if (!buffer_read_u32_be(fdt_buf, FDT_OFF_TOTAL_SIZE, &total_size) ||
        !buffer_read_u32_be(fdt_buf, FDT_OFF_OFF_DT_STRUCT, &struct_off) ||
        !buffer_read_u32_be(fdt_buf, FDT_OFF_OFF_DT_STRINGS, &strings_off) ||
        !buffer_read_u32_be(fdt_buf, FDT_OFF_SIZE_DT_STRUCT, &struct_size) ||
        !buffer_read_u32_be(fdt_buf, FDT_OFF_VERSION, &fdt_version))
        return make_buffer(nullptr, 0);

    if (fdt_version > FDT_COMPATIBLE_VERSION)
        return make_buffer(nullptr, 0);

    if (struct_off + struct_size > total_size)
        return make_buffer(nullptr, 0);

    u32 offset = struct_off + 4;

    return parse_subtree(&fdt_buf, &offset, struct_off + struct_size, strings_off, path + 1, p_name);

    // PARSING

    return make_buffer(nullptr, 0);
}
*/
