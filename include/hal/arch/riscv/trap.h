#ifndef HAL_ARCH_RISCV_TRAP_H
#define HAL_ARCH_RISCV_TRAP_H

#include <hal/trap.h>

/**
 * @addtogroup trap
 * @{
 */

/// @brief Encoded interrupt cause values from the RISC-V `scause` CSR.
typedef enum trap_interrupt_type {
	TRAP_INT_S_SOFTWARE = 1,
	TRAP_INT_S_TIMER = 5,
	TRAP_INT_S_EXTERNAL = 9,
	TRAP_INT_COUNTER_OVERFLOW = 13,
	// >=16 are for platform use
} trap_interrupt_type_t;

/** @brief Encoded exception cause values from the RISC-V `scause` CSR. */
typedef enum trap_exception_type {
	TRAP_EXC_INSTR_ADDRESS_MISALIGNED = 0,
	TRAP_EXC_INSTR_ACCESS_FAULT = 1,
	TRAP_EXC_ILLEGAL_INSTR = 2,
	TRAP_EXC_BREAKPOINT = 3,
	TRAP_EXC_LOAD_ADDRESS_MISALIGNED = 4,
	TRAP_EXC_LOAD_ACCESS_FAULT = 5,
	TRAP_EXC_STORE_ADDRESS_MISALIGNED = 6,
	TRAP_EXC_STORE_ACCESS_FAULT = 7,
	TRAP_EXC_ENV_CALL_U = 8,
	TRAP_EXC_ENV_CALL_S = 9,
	TRAP_EXC_INSTR_PAGE_FAULT = 12,
	TRAP_EXC_LOAD_PAGE_FAULT = 13,
	TRAP_EXC_STORE_PAGE_FAULT = 15,
	TRAP_EXC_SOFTWARE_CHECK = 18,
	TRAP_EXC_HARDWARE_ERROR = 19,
	// 24-31 designated for custom use
	// 48-63 designated for custom use
	// >=64 reserved
} trap_exception_type_t;

/**
 * @brief Saved trap context for RISC-V supervisor traps.
 */
struct trap_frame {
	union {
		/// General-purpose registers
		reg_t gpr[31];
		struct {
			reg_t ra;
			reg_t sp;
			reg_t gp;
			reg_t tp;
			reg_t t0;
			reg_t t1;
			reg_t t2;
			reg_t s0;
			reg_t s1;
			reg_t a0;
			reg_t a1;
			reg_t a2;
			reg_t a3;
			reg_t a4;
			reg_t a5;
			reg_t a6;
			reg_t a7;
			reg_t s2;
			reg_t s3;
			reg_t s4;
			reg_t s5;
			reg_t s6;
			reg_t s7;
			reg_t s8;
			reg_t s9;
			reg_t s10;
			reg_t s11;
			reg_t t3;
			reg_t t4;
			reg_t t5;
			reg_t t6;
		};
	};
	/// value of `sepc` CSR
	reg_t sepc;
	/// value of `sstatus` CSR
	reg_t sstatus;
	/// value of `stval` CSR
	reg_t stval;
	/// value of `scause` CSR
	reg_t scause;
};

static_assert(sizeof(trap_frame_t) == 35 * sizeof(reg_t));

/**
 * @brief Returns `true` if a trap cause corresponds to an interrupt.
 * @param cause Raw `scause` value.
 * @return `true` if the cause is an interrupt, `false` if it is an exception.
 */
bool trap_is_interrupt(reg_t cause);

/**
 * @brief Extracts interrupt code from `scause` by clearing the interrupt bit.
 * @param cause Raw `scause` value.
 * @return Interrupt code.
 */
trap_interrupt_type_t trap_get_interrupt_code(reg_t cause);

/**
 * @brief Returns exception code from `scause`.
 * @param cause Raw `scause` value.
 * @return Exception code.
 */
trap_exception_type_t trap_get_exception_code(reg_t cause);

/// @}

#endif // !HAL_ARCH_RISCV_TRAP_H
