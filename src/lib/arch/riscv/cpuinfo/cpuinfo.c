#include <arch/riscv/cpuinfo.h>

#include <dt/dt.h>
#include <stdbigos/buffer.h>
#include <stdbigos/error.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>
#include <stddef.h>
#include <stdint.h>

static_assert(RISCV_EXT_COUNT <= 64);

static const char* g_extension_names[RISCV_EXT_COUNT] = {
    "zifencei", "zicsr",      "zicntr",   "zihintntl", "zihintpause", "zimop",
    "zicond",   "zilsd",      "m",        "zmmul",     "a",           "zawrs",
    "zacas",    "zabha",      "zalasr",   "rvwmo",     "ztso",        "cmo",
    "f",        "d",          "q",        "zfh",       "zfhmin",      "bf16",
    "zfa",      "zfinx",      "zdinx",    "zhinx",     "zhinxmin",    "c",
    "zce",      "zclsd",      "b",        "v",         "zbkb",        "zbkc",
    "zbkx",     "zk",         "zks",      "zvbb",      "zvbc",        "zvkg",
    "zvkned",   "zvknhb",     "zvksed",   "zvksh",     "zvkt",        "zicfiss",
    "zicfilp",  "svade",      "svnapot",  "svpbmt",    "svinval",     "svadu",
    "svvptc",   "ssqosid",    "sstc",     "sscofpmf",  "ssdbltrp",
};

static char ascii_to_lower(char c) {
	if (c >= 'A' && c <= 'Z')
		return (char)(c + ('a' - 'A'));
	return c;
}

static bool strcmp_case_insensitive(const char* a, const char* b) {
	while (*a && *b) {
		if (ascii_to_lower(*a) != ascii_to_lower(*b))
			return false;
		++a;
		++b;
	}
	return *a == *b;
}

static void set_extension(riscv_hart_cpuinfo_t* info, riscv_extension_t ext) {
	if ((u32)ext >= (u32)RISCV_EXT_COUNT)
		return;
	info->extension_bitmap |= (1ULL << (u32)ext);
}

static bool find_extension(const char* token, riscv_extension_t* out_ext) {
	for (size_t i = 0; i < RISCV_EXT_COUNT; ++i) {
		if (strcmp_case_insensitive(token, g_extension_names[i])) {
			*out_ext = (riscv_extension_t)i;
			return true;
		}
	}
	return false;
}

static bool parse_hart_id_from_reg(buffer_t reg_buf, u32* out_hart_id) {
	if (!out_hart_id)
		return false;

	if (reg_buf.size == sizeof(u32)) {
		u32 reg = 0;
		if (!buffer_read_u32_be(reg_buf, 0, &reg))
			return false;
		*out_hart_id = reg;
		return true;
	}

	if (reg_buf.size == sizeof(u64)) {
		u64 reg = 0;
		if (!buffer_read_u64_be(reg_buf, 0, &reg) || reg > (u64)UINT32_MAX)
			return false;
		*out_hart_id = (u32)reg;
		return true;
	}

	return false;
}

static dt_node_t find_hart_node(const fdt_t* fdt, u32 hart_id) {
	dt_node_t cpus = dt_get_node_by_path(fdt, "/cpus");
	if (!cpus)
		return 0;

	for (dt_node_t node = dt_get_node_child(fdt, cpus); node; node = dt_get_node_sibling(fdt, node)) {
		dt_prop_t reg_prop = dt_get_prop_by_name(fdt, node, "reg");
		if (!reg_prop)
			continue;

		buffer_t reg_buf = dt_get_prop_buffer(fdt, reg_prop);
		u32 node_hart_id = 0;
		if (!parse_hart_id_from_reg(reg_buf, &node_hart_id))
			continue;

		if (node_hart_id == hart_id)
			return node;
	}

	return 0;
}

static void parse_isa_extensions_property(riscv_hart_cpuinfo_t* info, buffer_t ext_buf) {
	size_t offset = 0;

	while (offset < ext_buf.size) {
		const char* token = nullptr;
		u64 len = 0;
		if (!buffer_read_cstring_len(ext_buf, offset, &token, &len))
			break;

		riscv_extension_t ext;
		if (find_extension(token, &ext))
			set_extension(info, ext);

		offset += (size_t)len + 1;
	}
}

static bool parse_u32_prop(const fdt_t* fdt, dt_node_t node, const char* prop_name, u32* out_value) {
	if (!fdt || !node || !prop_name || !out_value)
		return false;

	dt_prop_t prop = dt_get_prop_by_name(fdt, node, prop_name);
	if (!prop)
		return false;

	buffer_t prop_buf = dt_get_prop_buffer(fdt, prop);
	if (prop_buf.size != sizeof(u32))
		return false;

	(void)buffer_read_u32_be(prop_buf, 0, out_value);
	return true;
}

error_t riscv_cpuinfo_from_fdt(const fdt_t* fdt, u32 hart_id, riscv_hart_cpuinfo_t* out) {
	if (!fdt || !buffer_is_valid(fdt->fdt_buffer) || !out)
		return ERR_BAD_ARG;

	memset(out, 0, sizeof(*out));
	out->hart_id = hart_id;

	dt_node_t cpu_node = find_hart_node(fdt, hart_id);
	if (!cpu_node)
		return ERR_NOT_FOUND;

	out->cpu_node = cpu_node;

	dt_prop_t isa_ext_prop = dt_get_prop_by_name(fdt, cpu_node, "riscv,isa-extensions");
	if (!isa_ext_prop)
		return ERR_NOT_FOUND;

	buffer_t isa_extensions = dt_get_prop_buffer(fdt, isa_ext_prop);
	parse_isa_extensions_property(out, isa_extensions);

	if (!parse_u32_prop(fdt, cpu_node, "riscv,cbop-block-size", &out->cbop_block_size))
		out->cbop_block_size = 0;
	if (!parse_u32_prop(fdt, cpu_node, "riscv,cboz-block-size", &out->cboz_block_size))
		out->cboz_block_size = 0;
	if (!parse_u32_prop(fdt, cpu_node, "riscv,cbom-block-size", &out->cbom_block_size))
		out->cbom_block_size = 0;

	return ERR_NONE;
}

bool riscv_cpuinfo_has_extension(const riscv_hart_cpuinfo_t* info, riscv_extension_t ext) {
	if (!info || (u32)ext >= (u32)RISCV_EXT_COUNT)
		return false;
	return (info->extension_bitmap & (1ULL << (u32)ext)) != 0;
}

const char* riscv_extension_name(riscv_extension_t ext) {
	if ((u32)ext >= (u32)RISCV_EXT_COUNT)
		return nullptr;
	return g_extension_names[(u32)ext];
}
