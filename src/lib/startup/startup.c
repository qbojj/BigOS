/*
   Simple C++ startup routine to setup CRT
   SPDX-License-Identifier: Unlicense

   (https://five-embeddev.com/ | http://www.shincbm.com/)

*/

#include <relocations/reloc.h>
#include <stdbigos/csr.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>
#include <stdint.h>

// Generic C function pointer.
typedef void (*function_t)(void);

// These symbols are defined by the linker script.
// See linker.lds

// NOLINTBEGIN(readability-identifier-naming)
extern u8 __bss_start [[gnu::weak]][];
extern u8 _end [[gnu::weak]][];

extern u8 __stack_start [[gnu::weak]][];

extern int main(u32 hartid, const void* fdt);

[[gnu::section(".fini"), noreturn, gnu::noinline]]
static void _Exit([[maybe_unused]] int return_code) {
	while (1) wfi();
}

// NOLINTBEGIN(clang-analyzer-security.ArrayBound)
[[noreturn, gnu::used]]
void _start_c(u32 hartid, const void* fdt) {
	if (!self_relocate()) {
		_Exit(-1);
	}
	if (__bss_start) {
		memset(__bss_start, 0, (uintptr_t)_end - (uintptr_t)__bss_start);
	}

	int rc = main(hartid, fdt);

	_Exit(rc);
}
// NOLINTEND(clang-analyzer-security.ArrayBound)
// NOLINTEND(readability-identifier-naming)
