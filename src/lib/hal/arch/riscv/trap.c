#include <stdbigos/error.h>
#include <stdbigos/math.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>
#include <stdint.h>
#include <hal/trap.h>
#include "csr_vals.h"
#include "csr.h"
#include "trap.h"

extern void hal_riscv_trap_entry();
extern void hal_riscv_trap_restore();

// Global registered handlers
static hal_timer_handler_t g_timer_handler = nullptr;
static hal_syscall_handler_t g_syscall_handler = nullptr;

static void hal_riscv_trap_interrupt_handler(hal_riscv_trap_interrupt_t code, riscv_trap_frame_t* ctx) {
	switch(code) {
	case HAL_RISCV_TRAP_INT_S_TIMER:
		if (g_timer_handler) {
			g_timer_handler();
		}
		break;
	default:
		// Unknown interrupt - for now just ignore
		break;
	}
}

static void hal_riscv_trap_exception_handler(hal_riscv_trap_exception_t code, riscv_trap_frame_t* ctx) {
	switch (code) {
	case HAL_RISCV_TRAP_EXC_ENV_CALL_U:
		if (g_syscall_handler) {
			// Extract syscall arguments from registers (as reg_t)
			reg_t syscall_id = ctx->a7;
			reg_t arg0 = ctx->a0;
			reg_t arg1 = ctx->a1;
			reg_t arg2 = ctx->a2;
			reg_t arg3 = ctx->a3;
			reg_t arg4 = ctx->a4;
			reg_t arg5 = ctx->a5;

			// Call handler and get result
			hal_syscall_result_t result = g_syscall_handler(syscall_id, arg0, arg1, arg2, arg3, arg4, arg5);

			// Store error in a0, value in a1
			ctx->a0 = result.error;
			ctx->a1 = result.value;
		}

		ctx->sepc += 4; // advance past ecall instruction
		break;
	default:
		// Unknown exception - for now just ignore
		break;
	}
}

void hal_riscv_trap_handler_trampoline(riscv_trap_frame_t* ctx) {
	// disable:
	// - floating point extensions
	// - vector extensions
	// - MXR - memory executable readable
	// - SUM - supervisor read userspace
	// - SDT - double trap detection
	CSR_CLEAR(sstatus, (CSR_SSTATUS_VS_MASK << CSR_SSTATUS_VS_OFFSET) | (CSR_SSTATUS_FS_MASK << CSR_SSTATUS_FS_OFFSET) |
	                       CSR_SSTATUS_MXR | CSR_SSTATUS_SUM | CSR_SSTATUS_SDT | 0);

	reg_t scause = ctx->scause;

	if (hal_riscv_trap_is_interrupt(scause)) {
		hal_riscv_trap_interrupt_handler(hal_riscv_trap_get_interrupt_code(scause), ctx);
	} else {
		hal_riscv_trap_exception_handler(hal_riscv_trap_get_exception_code(scause), ctx);
	}
}

error_t hal_trap_init(void) {
	CSR_WRITE(sscratch, 0);
	CSR_WRITE(stvec, hal_riscv_trap_entry);
	return ERR_NONE;
}

error_t hal_trap_register_timer_handler(hal_timer_handler_t handler) {
	g_timer_handler = handler;
	return ERR_NONE;
}

error_t hal_trap_register_syscall_handler(hal_syscall_handler_t handler) {
	g_syscall_handler = handler;
	return ERR_NONE;
}

__attribute__((noreturn))
void hal_trap_jump_to_userspace(uintptr_t user_sp, uintptr_t user_pc) {
	// Create a trap frame for user mode entry
	// This sets up the state as if we're returning from a trap
	riscv_trap_frame_t frame;
	memset(&frame, 0, sizeof(frame));

	// Set up registers
	frame.sp = user_sp;
	frame.sepc = user_pc;

	// Set sstatus for user mode:
	// SPP = 0 (user mode), SPIE = 1 (enable interrupts after sret)
	frame.sstatus = CSR_READ(sstatus);
	frame.sstatus &= ~CSR_SSTATUS_SPP;
	frame.sstatus |= CSR_SSTATUS_SPIE;

	// Jump to user mode using sret
	// We need to restore context and execute sret
	__asm__ volatile(
		// Load all registers from frame
		"mv a0, %0\n"              // a0 = &frame

		// Restore all GPRs from the frame
		"ld ra, 0(a0)\n"
		"ld sp, 8(a0)\n"
		"ld gp, 16(a0)\n"
		"ld tp, 24(a0)\n"
		"ld t0, 32(a0)\n"
		"ld t1, 40(a0)\n"
		"ld t2, 48(a0)\n"
		"ld s0, 56(a0)\n"
		"ld s1, 64(a0)\n"
		"ld a1, 80(a0)\n"
		"ld a2, 88(a0)\n"
		"ld a3, 96(a0)\n"
		"ld a4, 104(a0)\n"
		"ld a5, 112(a0)\n"
		"ld a6, 120(a0)\n"
		"ld a7, 128(a0)\n"
		"ld s2, 136(a0)\n"
		"ld s3, 144(a0)\n"
		"ld s4, 152(a0)\n"
		"ld s5, 160(a0)\n"
		"ld s6, 168(a0)\n"
		"ld s7, 176(a0)\n"
		"ld s8, 184(a0)\n"
		"ld s9, 192(a0)\n"
		"ld s10, 200(a0)\n"
		"ld s11, 208(a0)\n"
		"ld t3, 216(a0)\n"
		"ld t4, 224(a0)\n"
		"ld t5, 232(a0)\n"
		"ld t6, 240(a0)\n"

		// Load control registers (after other registers to avoid clobbering a0/a1)
		"ld a1, 248(a0)\n"         // sepc at offset 31*8
		"ld a2, 256(a0)\n"         // sstatus at offset 32*8

		// Load a0 last
		"ld a0, 72(a0)\n"

		// Set CSRs
		"csrw sepc, a1\n"
		"csrw sstatus, a2\n"

		// Jump to user mode
		"sret\n"

		: : "r"(&frame) : "memory"
	);

	__builtin_unreachable();
}

void hal_wait_for_interrupt(void) {
	__asm__ volatile("wfi" ::: "memory");
}
