#include "trap.h"

#include <stdbigos/csr.h>
#include <stdbigos/error.h>
#include <stdbigos/math.h>
#include <stdbigos/string.h>

extern void kernel_trap_entry();

error_t kernel_interrupt_init() {
	CSR_WRITE(sscratch, 0);
	CSR_WRITE(stvec, kernel_trap_entry);
	return ERR_NONE;
}

error_t prepare_stack_for_transition(void** stack, struct trap_frame* tf, void* thread_ptr) {
	void* sp = (void*)ALIGN_DOWN((uintptr_t)*stack, 16);
	sp = (void*)((void**)sp - 2);
	*(void**)sp = thread_ptr;

	sp = (struct trap_frame*)sp - 1;
	memmove(sp, tf, sizeof(*tf));
	*stack = sp;

	return ERR_NONE;
}
