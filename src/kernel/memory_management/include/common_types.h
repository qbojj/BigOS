#ifndef KERNEL_MEMORY_MANAGEMENT_COMMON_TYPES
#define KERNEL_MEMORY_MANAGEMENT_COMMON_TYPES

#include <stdbigos/array_sizes.h>
#include <stdbigos/types.h>
#include <stddef.h>
#include <stdint.h>

/**
 *	@ingroup kernel
 *	@ingroup kmm
 *
 *	memory_region_t represents a contiguous range of valid, addressable memory.
 * */
typedef struct {
	size_t size;
	void* addr __sized_by(size);
} memory_region_t;

/**
 *	@ingroup kernel
 *	@ingroup kmm
 *
 *	memory_area_t represents a range of memory, which isn't necessarily addressable.
 * */
typedef struct {
	size_t size;
	uintptr_t addr;
} memory_area_t;

[[nodiscard]]
memory_area_t memory_region_to_area(memory_region_t reg);

[[nodiscard]]
bool do_memory_areas_overlap(memory_area_t area1, memory_area_t area2);

/**
 *	@ingroup kernel
 *	@ingroup kmm
 *
 *  @brief Expands a memory area so that its start address and size are aligned to @p align.
 *
 *  The resulting area always fully contains the original @p area.
 *
 *  @param area The memory area to align.
 *  @param align The alignment in bytes. Must be non-zero.
 *
 *  @return A memory area aligned to @p align that completely encompasses @p area.
 *
 *  @note If @p align is a power of two, the dedicated *_pow2 variants are preferred.
 *
 *  @see memory_area_expand_to_alignment_pow2
 */
[[nodiscard]]
memory_area_t memory_area_expand_to_alignment(memory_area_t area, u64 align);

/**
 *	@ingroup kernel
 *	@ingroup kmm
 *
 *  @brief Shrinks a memory area so that its start address and size are aligned to @p align.
 *
 *  The resulting area is always fully contained in the original @p area.
 *
 *  @param area The memory area to align.
 *  @param align The alignment in bytes. Must be non-zero.
 *
 *  @return A memory area aligned to @p align that is completely encompassed by @p area.
 *
 *  @note If @p align is a power of two, the dedicated *_pow2 variants are preferred.
 *
 *  @see memory_area_shrink_to_alignment_pow2
 */
[[nodiscard]]
memory_area_t memory_area_shrink_to_alignment(memory_area_t area, u64 align);

/**
 *	@ingroup kernel
 *	@ingroup kmm
 *
 *  @brief Shrinks a memory region so that its start address and size are aligned to @p align.
 *
 *  The resulting region is always fully contained in the original @p area.
 *
 *  @param region The memory region to align.
 *  @param align The alignment in bytes. Must be non-zero.
 *
 *  @return A memory region aligned to @p align that is completely encompassed by @p region.
 *
 *  @note If @p align is a power of two, the dedicated *_pow2 variants are preferred.
 *
 *  @see memory_region_shrink_to_alignment_pow2
 */
[[nodiscard]]
memory_region_t memory_region_shrink_to_alignment(memory_region_t region, u64 align);

/**
 *	@ingroup kernel
 *	@ingroup kmm
 *
 *  @brief Expands a memory area so that its start address and size are aligned to 2^@p pow2.
 *
 *  The resulting area always fully contains the original @p region.
 *
 *  @param area The memory area to align.
 *  @param pow2 The power of 2 to align to.
 *
 *  @return A memory area aligned to 2^@p pow2 that completely encompasses @p area.
 *
 *  @see memory_area_expand_to_alignment
 */
[[nodiscard]]
memory_area_t memory_area_expand_to_alignment_pow2(memory_area_t area, u64 pow2);

/**
 *	@ingroup kernel
 *	@ingroup kmm
 *
 *  @brief Shrinks a memory region so that its start address and size are aligned to 2^@p pow2.
 *
 *  The resulting region is always fully contained in the original @p region.
 *
 *  @param area The memory area to align.
 *  @param pow2 The power of 2 to align to.
 *
 *  @return A memory area aligned to 2^@p pow2 that is completely encompassed by @p area.
 *
 *  @see memory_area_shrink_to_alignment
 */
[[nodiscard]]
memory_area_t memory_area_shrink_to_alignment_pow2(memory_area_t area, u64 pow2);

/**
 *	@ingroup kernel
 *	@ingroup kmm
 *
 *  @brief Shrinks a memory region so that its start address and size are aligned to 2^@p pow2.
 *
 *  The resulting region is always fully contained in the original @p region.
 *
 *  @param region The memory region to align.
 *  @param pow2 The power of 2 to align to.
 *
 *  @return A memory region aligned to 2^@p pow2 that is completely encompassed by @p region.
 *
 *  @see memory_region_shrink_to_alignment
 */
[[nodiscard]]
memory_region_t memory_region_shrink_to_alignment_pow2(memory_region_t region, u64 pow2);

#endif
