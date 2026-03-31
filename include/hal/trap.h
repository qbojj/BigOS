#ifndef HAL_TRAP_H
#define HAL_TRAP_H

#include <stdbigos/error.h>
#include <stdbigos/types.h>

/**
 * @addtogroup trap
 * @{
 */

/**
 * @brief Opaque trap context structure.
 *
 * Internal implementation detail - should not be accessed directly.
 */
typedef struct hal_trap_frame hal_trap_frame_t;

/**
 * @brief Timer interrupt handler callback.
 */
typedef void (*hal_timer_handler_t)(void);

/**
 * @brief Syscall return value structure.
 *
 * Holds both the error code and the return value from a syscall.
 * This allows architecture-independent representation of syscall results.
 */
typedef struct {
	reg_t error;  ///< Error code (0 for success, non-zero for error)
	reg_t value;  ///< Return value
} hal_syscall_result_t;

/**
 * @brief System call handler callback.
 *
 * Handles system calls (ecall/svc instructions). Parameters are
 * architecture-dependent register types.
 *
 * @param syscall_id System call number (from a7/r7)
 * @param arg0-arg5 System call arguments
 * @return Syscall result with error code and return value
 */
typedef hal_syscall_result_t (*hal_syscall_handler_t)(reg_t syscall_id, reg_t arg0, reg_t arg1, reg_t arg2, reg_t arg3, reg_t arg4, reg_t arg5);

/**
 * @brief Initialize trap handling subsystem.
 *
 * Sets up trap entry point and registers trap vector. Specific handlers
 * for timers and syscalls can be registered separately using dedicated
 * registration functions.
 *
 * @retval ERR_NONE success
 */
error_t hal_trap_init(void);

/**
 * @brief Register a timer interrupt handler.
 *
 * Installs a dedicated handler for timer interrupts to be called
 * in addition to (or instead of) the generic trap handler.
 *
 * @param handler Non-null timer handler, or NULL to disable.
 *
 * @retval ERR_NONE success
 */
error_t hal_trap_register_timer_handler(hal_timer_handler_t handler);

/**
 * @brief Register a system call handler.
 *
 * Installs a dedicated handler for system calls (ecall/svc instructions).
 * The handler receives the syscall ID and arguments as architecture-dependent
 * register types, and returns both an error code and value.
 *
 * @param handler Non-null syscall handler, or NULL to disable.
 *
 * @retval ERR_NONE success
 */
error_t hal_trap_register_syscall_handler(hal_syscall_handler_t handler);

/**
 * @brief Jump to user mode with specified program counter and stack.
 *
 * Configures the trap context to transition to user mode at the given
 * program counter with the given stack pointer. This is a one-way operation
 * that does not return.
 *
 * @param user_sp User-mode stack pointer (top of stack).
 * @param user_pc User-mode program counter (entry point).
 */
[[noreturn]]
void hal_trap_jump_to_userspace(uintptr_t user_sp, uintptr_t user_pc);

/**
 * @brief Wait for the next interrupt.
 *
 * Executes a wait-for-interrupt instruction. Control returns when an
 * enabled interrupt occurs and is processed.
 */
void hal_wait_for_interrupt(void);

/// @}

#endif // !HAL_TRAP_H
