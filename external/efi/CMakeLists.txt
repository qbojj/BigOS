project(gnuefi)

add_library(gnuefi INTERFACE)

set(EFI_DIR ${PROJECT_SOURCE_DIR})

set(LINKER_SCRIPT ${EFI_DIR}/lib/elf_riscv64_efi.lds)

target_link_options(gnuefi INTERFACE 
	-T ${LINKER_SCRIPT} -Bsymbolic ${EFI_DIR}/lib/crt0-efi-riscv64.o
	-nostdlib -znocombreloc)

target_compile_options(gnuefi INTERFACE
	-fpic -ffreestanding
	 -fno-stack-protector -fno-stack-check
	 -fshort-wchar -mabi=lp64d -march=rv64g)

target_include_directories(gnuefi INTERFACE ${EFI_DIR}/include/efi)
target_link_directories(gnuefi INTERFACE ${EFI_DIR}/lib)
target_link_libraries(gnuefi INTERFACE efi gnuefi)

# objcopy -j .text -j .sdata -j .data -j .rodata \
#         -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* \
#         -j .rela.* -j .reloc --target efi-app-$(ARCH) --subsystem=10 $^ $@

