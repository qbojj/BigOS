/*
   Simple C++ startup routine to setup CRT
   SPDX-License-Identifier: Unlicense

   (https://five-embeddev.com/ | http://www.shincbm.com/)

*/

#include <relocations/reloc.h>
#include <stdbigos/csr.h>
#include <stdbigos/string.h>
#include <stdbigos/types.h>

// Generic C function pointer.
typedef void (*function_t)(void);

// These symbols are defined by the linker script.
// See linker.lds

// NOLINTBEGIN(readability-identifier-naming)
extern u8 __bss_start [[gnu::weak]][];
extern u8 __bss_end [[gnu::weak]][];

extern function_t __preinit_array_start [[gnu::weak]][];
extern function_t __preinit_array_end [[gnu::weak]][];

extern function_t __init_array_start [[gnu::weak]][];
extern function_t __init_array_end [[gnu::weak]][];

extern function_t __fini_array_start [[gnu::weak]][];
extern function_t __fini_array_end [[gnu::weak]][];

extern int main(u32 hartid, const void* fdt);

[[gnu::section(".init"), gnu::naked]]
void _start(void) {
	__asm__ volatile(".option push\n\t"
	                 ".option norelax\n\t"
	                 "lla   gp, __global_pointer$\n\t"
	                 ".option pop\n\t"
	                 "lla   sp, __stack_start\n\t"
	                 "j     _start_c");
}

[[gnu::section(".fini"), noreturn, gnu::noinline]]
static void _Exit([[maybe_unused]] int return_code) {
	while (1) wfi();
}

// NOLINTBEGIN(clang-analyzer-security.ArrayBound)
[[gnu::section(".init")]]
static void _call_constructors() {
	for (const function_t* entry = __preinit_array_start; entry < __preinit_array_end; ++entry) {
		(*entry)();
	}

	for (const function_t* entry = __init_array_start; entry < __init_array_end; ++entry) {
		(*entry)();
	}
}

[[gnu::section(".fini")]]
static void _call_destructors() {
	for (const function_t* entry = __fini_array_start; entry < __fini_array_end; ++entry) {
		(*entry)();
	}
}

[[gnu::section(".init"), noreturn, gnu::used]]
static void _start_c(u32 hartid, const void* fdt) {
	if (!self_relocate()) {
		_Exit(-1);
	}
	if (__bss_start) {
		memset(__bss_start, 0, (uintptr_t)__bss_end - (uintptr_t)__bss_start);
	}
	_call_constructors();

	int rc = main(hartid, fdt);

	_call_destructors();
	_Exit(rc);
}
// NOLINTEND(clang-analyzer-security.ArrayBound)
// NOLINTEND(readability-identifier-naming)
