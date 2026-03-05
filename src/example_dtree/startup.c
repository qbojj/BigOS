/*
   Simple C++ startup routine to setup CRT
   SPDX-License-Identifier: Unlicense

   (https://five-embeddev.com/ | http://www.shincbm.com/)

*/

#include <stdbigos/csr.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>

// Generic C function pointer.
typedef void (*function_t)(void);

// NOLINTBEGIN

// These symbols are defined by the linker script.
// See linker.lds
extern u8 __bss_start [[gnu::weak]][];
extern u8 __bss_end [[gnu::weak]][];

// NOLINTEND

extern int main(u32 hartid, const void* fdt);

[[noreturn, gnu::noinline]]
void _Exit([[maybe_unused]] int return_code) { // NOLINT
	while (1) wfi();
}

[[noreturn, gnu::used]]
void _start(u32 hartid, const void* fdt) { // NOLINT
	size_t bss_size = (uintptr_t)__bss_end - (uintptr_t)__bss_start;
	memset(__bss_start, 0, bss_size);

	int rc = main(hartid, fdt);

	_Exit(rc);
}
