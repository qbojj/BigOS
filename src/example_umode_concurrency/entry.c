#include <debug/debug_stdio.h>
#include <stdbigos/csr.h>
#include <stdbigos/csr_vals.h>
#include <stdbigos/error.h>
#include <stdbigos/sbi.h>
#include <stdbigos/types.h>
#include <hal/arch/riscv/trap.h>

enum {
	SYSCALL_PRINT = 1,
};

// extracted from fdt /cpus/timebase-frequency
// QEMU tells 10MHz
#define TIMEBASE_FREQUENCY 10000000ul

#define TASK_COUNT 2

// switch 100 times per second
// sleep for 1 second each
#define TIMER_QUANTUM (u64)(TIMEBASE_FREQUENCY / 100)
#define SLEEP_TIME    (u64)(TIMEBASE_FREQUENCY * 1)
#define SIE_STIE      (1ul << HAL_RISCV_TRAP_INT_S_TIMER)

typedef struct {
	hal_riscv_trap_context_t context;
	bool initialized;
} task_ctx_t;

static task_ctx_t g_tasks[TASK_COUNT];
static u32 g_current_task = 0;
static u64 g_next_switch = 0;

static u8 g_user_mode_stack_a[4096] __attribute__((aligned(4096)));
static u8 g_user_mode_stack_b[4096] __attribute__((aligned(4096)));

static inline u64 read_time() {
	u64 now;
	__asm__ volatile("rdtime %0" : "=r"(now));
	return now;
}

static inline void set_timer(u64 timeout) {
#if __riscv_xlen == 64
	CSR_WRITE(stimecmp, timeout);
#elif __riscv_xlen == 32
	CSR_WRITE(stimecmp, (u32)timeout);
	CSR_WRITE(stimecmph, (u32)(timeout >> 32));
#else
	static_assert(false);
#endif
}

static void arm_next_timer() {
	g_next_switch = read_time() + TIMER_QUANTUM;
	set_timer(g_next_switch);
}

static inline void user_sys_print(const char* str) {
	register reg_t a0 __asm__("a0") = (reg_t)str;
	register reg_t a7 __asm__("a7") = SYSCALL_PRINT;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a7) : "memory");
}

[[noreturn]] void user_task_a() {
	while (true) {
		user_sys_print("[U-task A] running\n");
		u64 start = read_time();
		while (read_time() - start < SLEEP_TIME);
	}
}

[[noreturn]] void user_task_b() {
	while (true) {
		user_sys_print("[U-task B] running\n");
		u64 start = read_time();
		while (read_time() - start < SLEEP_TIME);
	}
}

static void init_task(task_ctx_t* task, void (*entry)(), void* user_stack_top) {
	hal_riscv_trap_context_clear(&task->context);
	hal_riscv_trap_context_set_sp((hal_trap_frame_t*)&task->context, (reg_t)user_stack_top);
	hal_riscv_trap_context_set_sepc((hal_trap_frame_t*)&task->context, (reg_t)entry);
	hal_riscv_trap_context_set_sstatus((hal_trap_frame_t*)&task->context, CSR_READ_RELAXED(sstatus));
	hal_riscv_trap_context_set_sstatus(
	    (hal_trap_frame_t*)&task->context,
	    (hal_riscv_trap_context_get_sstatus((hal_trap_frame_t*)&task->context) & ~CSR_SSTATUS_SPP) |
	        CSR_SSTATUS_SPIE);
	task->initialized = true;
}

static void handle_syscall(hal_trap_frame_t* frame) {
	hal_riscv_trap_context_set_sepc(frame, hal_riscv_trap_context_get_sepc(frame) + 4);

	switch (hal_riscv_trap_context_get_a7(frame)) {
	case SYSCALL_PRINT:
		dputs((const char*)hal_riscv_trap_context_get_a0(frame));
		hal_riscv_trap_context_set_a0(frame, 0);
		break;
	default:
		dprintf("unknown syscall id=%lu\n", (u64)hal_riscv_trap_context_get_a7(frame));
		hal_riscv_trap_context_set_a0(frame, (reg_t)-1);
		break;
	}
}

static void switch_to_next_task(hal_trap_frame_t* frame) {
	hal_riscv_trap_context_copy(&g_tasks[g_current_task].context, (const hal_riscv_trap_context_t*)frame);
	g_current_task = (g_current_task + 1) % TASK_COUNT;
	hal_riscv_trap_context_copy((hal_riscv_trap_context_t*)frame, &g_tasks[g_current_task].context);
}

void my_trap_handler(hal_trap_frame_t* frame) {
	const reg_t scause = hal_riscv_trap_context_get_scause(frame);
	if (hal_riscv_trap_is_interrupt(scause)) {
		if (hal_riscv_trap_get_interrupt_code(scause) == HAL_RISCV_TRAP_INT_S_TIMER) {
			if (read_time() < g_next_switch) {
				// Spurious timer interrupt, ignore.
				return;
			}
			arm_next_timer();
			switch_to_next_task(frame);
			return;
		}

		dprintf("unexpected interrupt %lu\n", (u64)hal_riscv_trap_get_interrupt_code(scause));
		return;
	}

	switch (hal_riscv_trap_get_exception_code(scause)) {
	case HAL_RISCV_TRAP_EXC_ENV_CALL_U: handle_syscall(frame); break;
	default:
		dprintf("unexpected exception %lu stval=%p\n",
		        (u64)hal_riscv_trap_get_exception_code(scause),
		        (void*)hal_riscv_trap_context_get_stval(frame));
		break;
	}
}

void main([[maybe_unused]] u32 hartid, [[maybe_unused]] const void* fdt) {
	if (hal_trap_init(my_trap_handler) != ERR_NONE) {
		dputs("trap_init failed\n");
		return;
	}

	init_task(&g_tasks[0], user_task_a, &g_user_mode_stack_a[sizeof(g_user_mode_stack_a)]);
	init_task(&g_tasks[1], user_task_b, &g_user_mode_stack_b[sizeof(g_user_mode_stack_b)]);

	dputs("starting U-mode concurrency example\n");
	CSR_SET(sie, SIE_STIE);
	arm_next_timer();

	// as our handlers are non-preemptive we only need one kernel stack
	// but if we wanted kernel preemption, we would need to have one stack
	// per task

	// we will piggyback of our current stack
	alignas(16) hal_riscv_trap_context_t context = g_tasks[0].context;
	hal_trap_restore_with_cleanup(&context, nullptr, nullptr);
}
