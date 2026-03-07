#ifndef KERNEL_TRAP_H_
#define KERNEL_TRAP_H_

#include <stdbigos/error.h>
#include <stdbigos/types.h>

typedef struct trap_frame {
	reg_t gpr[32];
	reg_t sepc;
	reg_t sstatus;
	reg_t scause;
	reg_t stval;
} trap_frame_t;

typedef void (*pfn_continuation_t)(void* user);

error_t kernel_interrupt_init();

[[gnu::nonnull]]
error_t prepare_stack_for_transition(void* restrict* restrict stack, const trap_frame_t* restrict tf);

[[noreturn, gnu::nonnull(1, 3)]]
void jump_with_stack(void* stack, void* user, pfn_continuation_t continuation);

[[noreturn, gnu::nonnull(1)]]
void kernel_restore_with_cleanup(void* stack, void* user, pfn_continuation_t cleanup);

#endif
