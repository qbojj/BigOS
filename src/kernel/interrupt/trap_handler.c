#include <debug/debug_stdio.h>
#include <stdbigos/csr.h>
#include <stdbigos/trap.h>
#include <stdbigos/types.h>

#include "trap.h"

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
	[[maybe_unused]] reg_t stval = CSR_READ_RELAXED(stval);

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
	reg_t scause = CSR_READ_RELAXED(scause);
	if (is_interrupt(scause)) {
		handle_irq(get_interrupt_code(scause));
	} else {
		handle_exc(get_exception_code(scause), tf);
	}
}
