#include <hal/irq.h>
#include <stdbigos/csr.h>

#define SSTATUS_SIE 0x2

void hal_disable_irq(void) {
	CSR_CLEAR(sstatus, SSTATUS_SIE);
}

void hal_enable_irq(void) {
	CSR_SET(sstatus, SSTATUS_SIE);
}
