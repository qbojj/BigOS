#include <elf.h>
#include <relocations/reloc.h>
#include <stdbigos/types.h>

#define R_RISCV_NONE     0
#define R_RISCV_RELATIVE 3

// NOLINTNEXTLINE
extern const reg_t _GLOBAL_OFFSET_TABLE_[] __attribute__((visibility("hidden")));

/* Get link time address of _DYNAMIC e.g first entry in GOT */
static inline Elf64_Addr elf_get_dynamic_linktime() {
	return _GLOBAL_OFFSET_TABLE_[0];
}

static inline Elf64_Dyn* elf_get_dynamic_runtime() {
	Elf64_Dyn* dynamic;
	__asm__("lla %0, _DYNAMIC" : "=r"(dynamic));
	return dynamic;
}

bool self_relocate() {
	Elf64_Dyn* dynamic = elf_get_dynamic_runtime();
	u8* load_addr = (u8*)((uintptr_t)dynamic - elf_get_dynamic_linktime());

	Elf64_Xword relasz = 0;
	Elf64_Xword relaent = 0;
	const u8* rela = nullptr;

	for (; dynamic->d_tag != DT_NULL; dynamic++) {
		switch (dynamic->d_tag) {
		case DT_RELA:    rela = load_addr + dynamic->d_un.d_ptr; break;
		case DT_RELASZ:  relasz = dynamic->d_un.d_val; break;
		case DT_RELAENT: relaent = dynamic->d_un.d_val; break;
		default:         break;
		}
	}

	if (!rela)
		return relaent == 0;

	if (relaent == 0)
		return rela == nullptr;

	for (Elf64_Xword off = 0; off < relasz; off += relaent) {
		const Elf64_Rela* cur_rel = (const Elf64_Rela*)(rela + off);
		switch (ELF64_R_TYPE(cur_rel->r_info)) {
		case R_RISCV_NONE: break;
		case R_RISCV_RELATIVE:
			void** addr = (void**)(load_addr + cur_rel->r_offset);
			*addr = load_addr + cur_rel->r_addend;
			break;
		default:
			// unknown relocation type
			return false;
		}
	}

	return true;
}
