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
 * Architecture-specific fields are defined in arch trap headers,
 * e.g. `hal/arch/riscv/trap.h`.
 */
typedef struct hal_trap_frame hal_trap_frame_t;

/**
 * @brief Trap handler callback executed by the trap trampoline.
 */
typedef void (*hal_trap_handler_t)(hal_trap_frame_t* frame);
/**
 * @brief Continuation callback used by stack transition helpers.
 */
typedef void (*hal_continuation_t)(void* user);

/**
 * @brief Installs trap entry and sets kernel trap callback.
 *
 * @param handler Non-null C trap handler.
 *
 * @retval ERR_NONE success
 */
[[gnu::nonnull]]
error_t hal_trap_init(hal_trap_handler_t handler);

/**
 * @brief Prepares a stack so trap restore can continue from a supplied frame.
 *
 * @param stack In/out top of the kernel stack pointer after the transition. Updated to point to the new top after
 * pushing the trap frame.
 * @param tf Trap frame copied onto the target stack.
 *
 * @retval ERR_NONE success
 */
[[gnu::nonnull]]
error_t hal_trap_prepare_stack_for_transition(void** stack, const hal_trap_frame_t* frame);

/**
 * @brief Switches to `stack` and transfers control to `continuation(user)`.
 *
 * This is a helper routine for performing stack/context transitions, e.g. when
 * migrating from the boot stack to a per-hart kernel stack. The `continuation`
 * callback is executed after the stack switch, and receives a user-provided pointer for context.
 *
 * @param stack Top of the kernel stack to switch to.
 * @param user User-provided pointer passed to the continuation callback.
 * @param continuation Non-null callback executed after the stack switch.
 */
[[noreturn, gnu::nonnull(1, 3)]]
void hal_trap_jump_with_stack(void* stack, void* user, hal_continuation_t continuation);

/**
 * @brief Restores trap context from `stack`, optionally running `cleanup(user)` first on the new stack.
 *
 * This is a helper routine for performing trap return with an optional cleanup step. If `cleanup` is non-null,
 * it is executed on the new stack before performing the trap return to user mode.
 * The `user` pointer is passed to the cleanup callback for context.
 *
 * @param stack Top of the kernel stack containing the trap frame to restore.
 * @param user User-provided pointer passed to the cleanup callback.
 * @param cleanup Optional callback executed before trap return. If null, the trap return is performed immediately
 * without cleanup.
 */
[[noreturn, gnu::nonnull(1)]]
void hal_trap_restore_with_cleanup(void* stack, void* user, hal_continuation_t cleanup);

/// @}

#endif // !HAL_TRAP_H
