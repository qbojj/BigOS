#include <stdbigos/csr.h>
#include <stdbigos/error.h>
#include <stdbigos/math.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>
#include <stdint.h>
#include <hal/arch/riscv/trap.h>

extern void hal_riscv_trap_entry();

typedef struct {
	union {
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
	reg_t sepc;
	reg_t sstatus;
	reg_t stval;
	reg_t scause;
} riscv_trap_frame_t;

static_assert(sizeof(riscv_trap_frame_t) == sizeof(hal_riscv_trap_context_t));

static inline riscv_trap_frame_t* riscv_frame(hal_trap_frame_t* context) {
	return (riscv_trap_frame_t*)context;
}

static inline const riscv_trap_frame_t* riscv_frame_const(const hal_trap_frame_t* context) {
	return (const riscv_trap_frame_t*)context;
}

static hal_trap_handler_t g_trap_handler = nullptr;

void hal_riscv_trap_handler_trampoline(hal_trap_frame_t* frame) {
	// disable:
	// - floating point extensions
	// - vector extensions
	// - MXR - memory executable readable
	// - SUM - supervisor read userspace
	// - SDT - double trap detection
	CSR_CLEAR(sstatus, (CSR_SSTATUS_VS_MASK << CSR_SSTATUS_VS_OFFSET) | (CSR_SSTATUS_FS_MASK << CSR_SSTATUS_FS_OFFSET) |
	                       CSR_SSTATUS_MXR | CSR_SSTATUS_SUM | CSR_SSTATUS_SDT | 0);
	if (g_trap_handler)
		g_trap_handler(frame);
}

error_t hal_trap_init(hal_trap_handler_t handler) {
	g_trap_handler = handler;
	CSR_WRITE(sscratch, 0);
	CSR_WRITE(stvec, hal_riscv_trap_entry);
	return ERR_NONE;
}

error_t hal_trap_prepare_stack_for_transition(void** stack, const hal_trap_frame_t* frame) {
	u8* sp = (u8*)ALIGN_DOWN((uintptr_t)*stack, 16);
	sp = sp - ALIGN_UP(16 + sizeof(riscv_trap_frame_t), 16); // leave space for tp and trap frame
	memcpy(sp, frame, sizeof(riscv_trap_frame_t));
	*stack = sp;

	return ERR_NONE;
}

bool hal_riscv_trap_is_interrupt(reg_t cause) {
	return (ireg_t)cause < 0;
}

hal_riscv_trap_interrupt_t hal_riscv_trap_get_interrupt_code(reg_t cause) {
	return (cause << 1) >> 1; // strip highest bit
}

hal_riscv_trap_exception_t hal_riscv_trap_get_exception_code(reg_t cause) {
	return cause;
}

void hal_riscv_trap_context_clear(hal_riscv_trap_context_t* context) {
	memset(context, 0, sizeof(*context));
}

void hal_riscv_trap_context_copy(hal_riscv_trap_context_t* dst, const hal_riscv_trap_context_t* src) {
	memcpy(dst, src, sizeof(*dst));
}

reg_t hal_riscv_trap_context_get_scause(const hal_trap_frame_t* context) {
	return riscv_frame_const(context)->scause;
}

reg_t hal_riscv_trap_context_get_stval(const hal_trap_frame_t* context) {
	return riscv_frame_const(context)->stval;
}

reg_t hal_riscv_trap_context_get_sstatus(const hal_trap_frame_t* context) {
	return riscv_frame_const(context)->sstatus;
}

void hal_riscv_trap_context_set_sstatus(hal_trap_frame_t* context, reg_t value) {
	riscv_frame(context)->sstatus = value;
}

reg_t hal_riscv_trap_context_get_sepc(const hal_trap_frame_t* context) {
	return riscv_frame_const(context)->sepc;
}

void hal_riscv_trap_context_set_sepc(hal_trap_frame_t* context, reg_t value) {
	riscv_frame(context)->sepc = value;
}

reg_t hal_riscv_trap_context_get_sp(const hal_trap_frame_t* context) {
	return riscv_frame_const(context)->sp;
}

void hal_riscv_trap_context_set_sp(hal_trap_frame_t* context, reg_t value) {
	riscv_frame(context)->sp = value;
}

reg_t hal_riscv_trap_context_get_a0(const hal_trap_frame_t* context) {
	return riscv_frame_const(context)->a0;
}

void hal_riscv_trap_context_set_a0(hal_trap_frame_t* context, reg_t value) {
	riscv_frame(context)->a0 = value;
}

reg_t hal_riscv_trap_context_get_a7(const hal_trap_frame_t* context) {
	return riscv_frame_const(context)->a7;
}

void hal_riscv_trap_context_set_a7(hal_trap_frame_t* context, reg_t value) {
	riscv_frame(context)->a7 = value;
}
