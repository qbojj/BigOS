if(RISCV_CLANG_TOOLCHAIN_INCLUDED)
  return()
endif()
set(RISCV_CLANG_TOOLCHAIN_INCLUDED TRUE)

include(${CMAKE_CURRENT_LIST_DIR}/riscv64-common.cmake)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    execute_process(
        COMMAND brew --prefix llvm
        OUTPUT_VARIABLE LLVM_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CMAKE_PREFIX_PATH "${LLVM_PREFIX}")
    set(CMAKE_C_USING_LINKER_LLD "-fuse-ld=lld")
    set(CMAKE_ASM_USING_LINKER_LLD "-fuse-ld=lld")
    set(CMAKE_LINKER_TYPE LLD)
endif()

find_program(CMAKE_C_COMPILER clang REQUIRED)
find_program(CMAKE_ASM_COMPILER clang REQUIRED)