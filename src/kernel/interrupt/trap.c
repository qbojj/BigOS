#include "trap.h"

#include <stdbigos/csr.h>
#include <stdbigos/error.h>
#include <stdbigos/math.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>
#include <stdint.h>

extern void kernel_trap_entry();

error_t kernel_interrupt_init() {
	CSR_WRITE(sscratch, 0);
	CSR_WRITE(stvec, kernel_trap_entry);
	return ERR_NONE;
}

error_t prepare_stack_for_transition(void* restrict* restrict stack, const trap_frame_t* restrict tf) {
	u8* sp = (u8*)ALIGN_DOWN((uintptr_t)*stack, 16);
	*stack = sp - 16 - sizeof(*tf); // leave space for tp and trap frame
	memcpy(*stack, tf, sizeof(*tf));

	return ERR_NONE;
}
