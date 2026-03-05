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
error_t prepare_stack_for_transition(void** stack, trap_frame_t* tf, void* thread_ptr);

[[noreturn]]
void jump_with_stack(void* stack, void* user, pfn_continuation_t continuation);

[[noreturn]]
void kernel_restore_with_cleanup(void* stack, void* user, pfn_continuation_t cleanup);

#endif
