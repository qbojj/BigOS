#include "trap.h"

#include <debug/debug_stdio.h>
#include <stdbigos/csr.h>
#include <stdbigos/trap.h>
#include <stdbigos/types.h>

void handle_irq(InterruptType irq) {
	switch (irq) {
	case IntSTimer:
	case IntSSoftware:
	case IntSExternal:
	case IntCounterOverflow: return;

	default:
		// we've got reserved signal
		DEBUG_PRINTF("interrupt: got reserved number %lu\n", (u64)irq);
		return;
	}
}

void handle_exc(ExceptionType exc, trap_frame_t* tf) {
	switch (exc) {
	case ExcEnvCallU:
	case ExcEnvCallS:
		intr_enable();
		tf->sepc += 4; // skip ECALL instruction

		// do something, registers are available in tf

		intr_disable();
		return;
	default:
		// we've got reserved exception
		DEBUG_PRINTF("exception: got reserved number %lu\n", (u64)exc);
		return;
	}
}

void kernel_trap_handler(trap_frame_t* tf) {
	if (is_interrupt(tf->scause)) {
		handle_irq(get_interrupt_code(tf->scause));
	} else {
		handle_exc(get_exception_code(tf->scause), tf);
	}
}
