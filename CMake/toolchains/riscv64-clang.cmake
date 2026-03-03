cmake_minimum_required(VERSION 3.13)

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
    set(CMAKE_ASM_COMPILER "${LLVM_PREFIX}/bin/clang")
    set(CMAKE_C_COMPILER "${LLVM_PREFIX}/bin/clang")
    set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=lld")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fuse-ld=lld")
else()
    set(CMAKE_ASM_COMPILER clang)
    set(CMAKE_C_COMPILER clang)
endif()
