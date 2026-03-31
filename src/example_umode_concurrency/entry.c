#include <debug/debug_stdio.h>
#include <stdbigos/error.h>
#include <stdbigos/types.h>
#include <hal/trap.h>

/**
 * Simple user mode example using the simplified public HAL API.
 *
 * This demonstrates basic syscall handling from user mode.
 * For a more advanced example with task switching, internal APIs would be needed.
 */

enum {
	SYSCALL_PRINT = 1,
};

static u8 g_user_mode_stack1[4096] __attribute__((aligned(4096)));
static u8 g_user_mode_stack2[4096] __attribute__((aligned(4096)));

/**
 * User-mode task that makes syscalls.
 */
[[noreturn]]
void user_task1(void) {
	while (true) {
		// Make a syscall: a7=1 (SYSCALL_PRINT), a0=string pointer
		register u64 result __asm__("a0") = (reg_t)"[User 1] Hello from user mode!\n";
		register u64 id __asm__("a7") = SYSCALL_PRINT;
		__asm__ volatile("ecall" : "+r"(result) : "r"(id) : );
	}
}

[[noreturn]]
void user_task2(void) {
	while (true) {
		// Make a syscall: a7=1 (SYSCALL_PRINT), a0=string pointer
		register u64 result __asm__("a0") = (reg_t)"[User 2] Hello from user mode!\n";
		register u64 id __asm__("a7") = SYSCALL_PRINT;
		__asm__ volatile("ecall" : "+r"(result) : "r"(id) : );
	}
}

/**
 * Syscall handler - processes system calls from user mode.
 * Receives syscall arguments as reg_t (architecture-dependent).
 * Returns error code and value - error in a0, value in a1.
 */
hal_syscall_result_t syscall_handler(reg_t syscall_id, reg_t arg0, reg_t arg1, reg_t arg2, reg_t arg3, reg_t arg4, reg_t arg5) {
	(void)arg1; (void)arg2; (void)arg3; (void)arg4; (void)arg5;
	switch (syscall_id) {
	case SYSCALL_PRINT:
		dputs((const char*)arg0);
		return (hal_syscall_result_t){.error = 0, .value = 0};
	default:
		dprintf("[Kernel] Unknown syscall: %lu\n", (u64)syscall_id);
		return (hal_syscall_result_t){.error = 1, .value = 0};
	}
}

void main([[maybe_unused]] u32 hartid, [[maybe_unused]] const void* fdt) {
	// Initialize the trap handling subsystem
	if (hal_trap_init() != ERR_NONE) {
		dputs("trap_init failed\n");
		return;
	}

	// Register the syscall handler
	if (hal_trap_register_timer_handler(timer_handler) != ERR_NONE) {
		dputs("timer handler registration failed\n");
		return;
	}

	if (hal_trap_register_syscall_handler(syscall_handler) != ERR_NONE) {
		dputs("syscall handler registration failed\n");
		return;
	}

	dputs("Jumping to user mode...\n");

	void* user_stack_top = &g_user_mode_stack1[sizeof(g_user_mode_stack1)];
	hal_trap_jump_to_userspace((uintptr_t)user_stack_top, (uintptr_t)user_task1);
}
