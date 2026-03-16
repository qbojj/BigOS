#ifndef BIGOS_INCLUDE_DT_RISCV_CPUINFO_H
#define BIGOS_INCLUDE_DT_RISCV_CPUINFO_H

#include <dt/dt.h>
#include <stdbigos/buffer.h>
#include <stdbigos/error.h>
#include <stdbigos/types.h>

/**
 * @brief Ratified RISC-V extensions.
 */
typedef enum riscv_extension_t {
	RISCV_EXT_ZIFENCEI = 0,
	RISCV_EXT_ZICSR,
	RISCV_EXT_ZICNTR,
	RISCV_EXT_ZIHINTNTL,
	RISCV_EXT_ZIHINTPAUSE,
	RISCV_EXT_ZIMOP,
	RISCV_EXT_ZICOND,
	RISCV_EXT_ZILSD,
	RISCV_EXT_M,
	RISCV_EXT_ZMMUL,
	RISCV_EXT_A,
	RISCV_EXT_ZAWRS,
	RISCV_EXT_ZACAS,
	RISCV_EXT_ZABHA,
	RISCV_EXT_ZALASR,
	RISCV_EXT_RVWMO,
	RISCV_EXT_ZTSO,
	RISCV_EXT_CMO,
	RISCV_EXT_F,
	RISCV_EXT_D,
	RISCV_EXT_Q,
	RISCV_EXT_ZFH,
	RISCV_EXT_ZFHMIN,
	RISCV_EXT_BF16,
	RISCV_EXT_ZFA,
	RISCV_EXT_ZFINX,
	RISCV_EXT_ZDINX,
	RISCV_EXT_ZHINX,
	RISCV_EXT_ZHINXMIN,
	RISCV_EXT_C,
	RISCV_EXT_ZCE,
	RISCV_EXT_ZCLSD,
	RISCV_EXT_B,
	RISCV_EXT_V,
	RISCV_EXT_ZBKB,
	RISCV_EXT_ZBKC,
	RISCV_EXT_ZBKX,
	RISCV_EXT_ZK,
	RISCV_EXT_ZKS,
	RISCV_EXT_ZVBB,
	RISCV_EXT_ZVBC,
	RISCV_EXT_ZVKG,
	RISCV_EXT_ZVKNED,
	RISCV_EXT_ZVKNHB,
	RISCV_EXT_ZVKSED,
	RISCV_EXT_ZVKSH,
	RISCV_EXT_ZVKT,
	RISCV_EXT_ZICFISS,
	RISCV_EXT_ZICFILP,
	RISCV_EXT_SVADE,
	RISCV_EXT_SVNAPOT,
	RISCV_EXT_SVPBMT,
	RISCV_EXT_SVINVAL,
	RISCV_EXT_SVADU,
	RISCV_EXT_SVVPTC,
	RISCV_EXT_SSQOSID,
	RISCV_EXT_SSTC,
	RISCV_EXT_SSCOFPMF,
	RISCV_EXT_SSDBLTRP,
	RISCV_EXT_COUNT,
} riscv_extension_t;

/**
 * @brief Parsed hart CPU information from the flattened device tree.
 */
typedef struct riscv_hart_cpuinfo_t {
	u32 hart_id;
	dt_node_t cpu_node;
	u64 extension_bitmap;
	u32 cbop_block_size;
	u32 cboz_block_size;
	u32 cbom_block_size;
} riscv_hart_cpuinfo_t;

/**
 * @brief Fill @p out with CPU info for @p hart_id from @p fdt.
 * @return ERR_NONE on success, ERR_BAD_ARG if arguments are invalid,
 *         ERR_NOT_FOUND if no CPU node matches @p hart_id.
 */
error_t riscv_cpuinfo_from_fdt(const fdt_t* fdt, u32 hart_id, riscv_hart_cpuinfo_t* out);

/**
 * @brief Check whether a parsed hart supports extension @p ext.
 */
bool riscv_cpuinfo_has_extension(const riscv_hart_cpuinfo_t* info, riscv_extension_t ext);

/**
 * @brief Get canonical lowercase name for extension @p ext.
 * @return nullptr if @p ext is out of range.
 */
const char* riscv_extension_name(riscv_extension_t ext);

#endif // BIGOS_INCLUDE_DT_RISCV_CPUINFO_H
