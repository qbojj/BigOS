#include <debug/debug_stdio.h>
#include <stdbigos/csr.h>
#include <stdbigos/string.h>
#include <stdbigos/trap.h>
#include <stdbigos/types.h>

// NOLINTBEGIN(readability-identifier-naming)
extern u8 bss_start[];
extern u8 bss_end[];
// NOLINTEND(readability-identifier-naming)

static const u64 g_clint_base = 0x02000000;

static volatile u64* g_mtime = (u64*)(g_clint_base + 0xBFF8);
static volatile u64* g_mtimecmp = (u64*)(g_clint_base + 0x4000);

static const u64 g_quant = 50000llu;

void main() {
	for (u32 i = 0;; ++i) dprintf("hello OS %u\n", i);
}

[[gnu::interrupt("machine")]]
void int_handler() {
	reg_t cause = CSR_READ(mcause);
	if (is_interrupt(cause)) {
		// interrupt
		reg_t int_no = get_interrupt_code(cause);

		switch (int_no) {
		case IntMTimer:
			dputs("\n\tgot timer interrupt\n");
			g_mtimecmp[hartid()] = *g_mtime + g_quant;
			break;
		default: dprintf("\n\tunknown interrupt (%ld)\n", int_no); break;
		}

		CSR_CLEAR(mip, (reg_t)1 << int_no);
		return;
	}
}

[[noreturn, gnu::used]]
void start() {
	const size_t bss_sz = (uintptr_t)bss_end - (uintptr_t)bss_start;

	memset(bss_start, '\0', bss_sz);

	// register handler
	CSR_WRITE(mtvec, int_handler);

	// request a timer interrupt
	g_mtimecmp[hartid()] = *g_mtime + g_quant;

	// set MIE in mstatus
	CSR_SET(mstatus, 8);

	// set TIMER in mie
	CSR_SET(mie, 1lu << IntMTimer);

	main();

	while (true) wfi();
}

[[gnu::section(".init"), gnu::naked]]
void _start() { // NOLINT(readability-identifier-naming)
	__asm__(".option push\n\t"
	        ".option norelax\n\t"
	        "la    gp, __global_pointer$\n\t"
	        ".option pop\n\t"
	        "la    sp, __stack_top\n\t"
	        "j start");
}
