#ifndef STDBIGOS_TRAP_H_
#define STDBIGOS_TRAP_H_

#include "types.h"

typedef enum InterruptType {
	IntSSoftware = 1,
	IntSTimer = 5,
	IntSExternal = 9,
	IntCounterOverflow = 13,
	// >=16 are for platform use
} InterruptType;

typedef enum ExceptionType {
	ExcInstrAddressMisaligned = 0,
	ExcInstrAccessFault = 1,
	ExcIllegalInstr = 2,
	ExcBreakpoint = 3,
	ExcLoadAddrMisaligned = 4,
	ExcLoadAccessFault = 5,
	ExcStoreAddressMisaligned = 6,
	ExcStoreAccessFault = 7,
	ExcEnvCallU = 8,
	ExcEnvCallS = 9,
	ExcInstrPageFault = 12,
	ExcLoadPageFault = 13,
	ExcSoftwareCheck = 18,
	ExcHardwareError = 19,
	// 24-31 designated for custom use
	// 48-63 designated for custom use
	// >=64 reserved
} ExceptionType;

static inline bool is_interrupt(reg_t cause) {
	return (ireg_t)cause < 0;
}

static inline InterruptType get_interrupt_code(reg_t cause) {
	return (cause << 1) >> 1; // strip highest bit
}

static inline ExceptionType get_exception_code(reg_t cause) {
	return cause;
}

#endif
