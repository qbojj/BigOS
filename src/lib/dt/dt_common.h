/**
 * @ingroup dt_internal
 * @file dt_common.h
 * @brief Common device tree parsing utilities.
 */

#ifndef SRC_LIB_DT_COMMON
#define SRC_LIB_DT_COMMON

#include <dt/dt.h>
#include <stdbigos/error.h>
#include <stdbigos/types.h>

// Offsets are of type u32

/// @addtogroup dt_internal
/// @ingroup dt
/// @{

/**
 * @brief Skip node's tag and name and output first aligned offset after them.
 * @param fdt Pointer to the flattened device tree.
 * @param node The device tree node.
 * @param[out] out_offset First aligned offset after the @p node.
 * @return
 * - ERR_NONE on success
 * - ERR_BAD_ARG on nullptr args or misaligned @p node
 * - ERR_NOT_VALID if the FDT is invalid
 */
[[nodiscard]]
error_t dt_skip_node_name(const fdt_t* fdt, dt_node_t node, u32* out_offset);

/**
 * @brief Skip node's properties starting at offset (if they exist) and output first aligned offset after them.
 * @param fdt Pointer to the flattened device tree.
 * @param offset The offset to start skipping properties from.
 * @param[out] out_offset First aligned offset after properties at @p offset.
 * @return
 * - ERR_NONE on success
 * - ERR_BAD_ARG on nullptr args
 * - ERR_NOT_VALID if the FDT is invalid
 */
[[nodiscard]]
error_t dt_skip_node_properties(const fdt_t* fdt, u32 offset, u32* out_offset);

/**
 * @brief Skip node's tag, name and properties and output first aligned offset after them.
 * @param fdt Pointer to the flattened device tree.
 * @param node The device tree node.
 * @param[out] out_offset First aligned offset after the @p node.
 * @return
 * - ERR_NONE on success
 * - ERR_BAD_ARG on nullptr args or misaligned @p node
 * - ERR_NOT_VALID if the FDT is invalid
 */
[[nodiscard]]
error_t dt_skip_node_header(const fdt_t* fdt, dt_node_t node, u32* out_offset);

/**
 * @brief Skip an overarching node's nested nodes starting at the start of the first nested node.
 * @param fdt Pointer to the flattened device tree.
 * @param nested_node The first nested node to start skipping from.
 * @param[out] out_node First non-nested node after the @p nested_node.
 * @return
 * - ERR_NONE on success
 * - ERR_BAD_ARG on nullptr args or misaligned @p nested_node
 * - ERR_NOT_VALID if the FDT is invalid
 */
[[nodiscard]]
error_t dt_skip_nested_nodes(const fdt_t* fdt, dt_node_t nested_node, dt_node_t* out_node);

/// @}

#endif // !SRC_LIB_DT_COMMON
