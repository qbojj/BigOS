#ifndef HAL_ARCH_RISCV_TRAP_H
#define HAL_ARCH_RISCV_TRAP_H

#include <hal/trap.h>
#include <stdbigos/types.h>

/**
 * @addtogroup trap
 * @{
 */

/// @brief Encoded interrupt cause values from the RISC-V `scause` CSR.
typedef enum hal_riscv_trap_interrupt {
	HAL_RISCV_TRAP_INT_S_SOFTWARE = 1,
	HAL_RISCV_TRAP_INT_S_TIMER = 5,
	HAL_RISCV_TRAP_INT_S_EXTERNAL = 9,
	HAL_RISCV_TRAP_INT_COUNTER_OVERFLOW = 13,
	// >=16 are for platform use
} hal_riscv_trap_interrupt_t;

/** @brief Encoded exception cause values from the RISC-V `scause` CSR. */
typedef enum hal_riscv_trap_exception {
	HAL_RISCV_TRAP_EXC_INSTR_ADDRESS_MISALIGNED = 0,
	HAL_RISCV_TRAP_EXC_INSTR_ACCESS_FAULT = 1,
	HAL_RISCV_TRAP_EXC_ILLEGAL_INSTR = 2,
	HAL_RISCV_TRAP_EXC_BREAKPOINT = 3,
	HAL_RISCV_TRAP_EXC_LOAD_ADDRESS_MISALIGNED = 4,
	HAL_RISCV_TRAP_EXC_LOAD_ACCESS_FAULT = 5,
	HAL_RISCV_TRAP_EXC_STORE_ADDRESS_MISALIGNED = 6,
	HAL_RISCV_TRAP_EXC_STORE_ACCESS_FAULT = 7,
	HAL_RISCV_TRAP_EXC_ENV_CALL_U = 8,
	HAL_RISCV_TRAP_EXC_ENV_CALL_S = 9,
	HAL_RISCV_TRAP_EXC_INSTR_PAGE_FAULT = 12,
	HAL_RISCV_TRAP_EXC_LOAD_PAGE_FAULT = 13,
	HAL_RISCV_TRAP_EXC_STORE_PAGE_FAULT = 15,
	HAL_RISCV_TRAP_EXC_SOFTWARE_CHECK = 18,
	HAL_RISCV_TRAP_EXC_HARDWARE_ERROR = 19,
	// 24-31 designated for custom use
	// 48-63 designated for custom use
	// >=64 reserved
} hal_riscv_trap_exception_t;

typedef struct {
	alignas(16) u8 data[35 * sizeof(reg_t)];
} hal_riscv_trap_context_t;

static_assert(sizeof(hal_riscv_trap_context_t) == 35 * sizeof(reg_t));

/**
 * @brief Returns `true` if a trap cause corresponds to an interrupt.
 * @param cause Raw `scause` value.
 * @return `true` if the cause is an interrupt, `false` if it is an exception.
 */
bool hal_riscv_trap_is_interrupt(reg_t cause);

/**
 * @brief Extracts interrupt code from `scause` by clearing the interrupt bit.
 * @param cause Raw `scause` value.
 * @return Interrupt code.
 */
hal_riscv_trap_interrupt_t hal_riscv_trap_get_interrupt_code(reg_t cause);

/**
 * @brief Returns exception code from `scause`.
 * @param cause Raw `scause` value.
 * @return Exception code.
 */
hal_riscv_trap_exception_t hal_riscv_trap_get_exception_code(reg_t cause);

void hal_riscv_trap_context_clear(hal_riscv_trap_context_t* context);
void hal_riscv_trap_context_copy(hal_riscv_trap_context_t* dst, const hal_riscv_trap_context_t* src);

reg_t hal_riscv_trap_context_get_scause(const hal_trap_frame_t* context);
reg_t hal_riscv_trap_context_get_stval(const hal_trap_frame_t* context);
reg_t hal_riscv_trap_context_get_sstatus(const hal_trap_frame_t* context);
void hal_riscv_trap_context_set_sstatus(hal_trap_frame_t* context, reg_t value);
reg_t hal_riscv_trap_context_get_sepc(const hal_trap_frame_t* context);
void hal_riscv_trap_context_set_sepc(hal_trap_frame_t* context, reg_t value);
reg_t hal_riscv_trap_context_get_sp(const hal_trap_frame_t* context);
void hal_riscv_trap_context_set_sp(hal_trap_frame_t* context, reg_t value);
reg_t hal_riscv_trap_context_get_a0(const hal_trap_frame_t* context);
void hal_riscv_trap_context_set_a0(hal_trap_frame_t* context, reg_t value);
reg_t hal_riscv_trap_context_get_a7(const hal_trap_frame_t* context);
void hal_riscv_trap_context_set_a7(hal_trap_frame_t* context, reg_t value);

/// @}

#endif // !HAL_ARCH_RISCV_TRAP_H
