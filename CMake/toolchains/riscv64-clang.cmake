cmake_minimum_required(VERSION 3.13)

if(RISCV_CLANG_TOOLCHAIN_INCLUDED)
  return()
endif()
set(RISCV_CLANG_TOOLCHAIN_INCLUDED TRUE)

set(RISCV_LINKER_FLAGS "${RISCV_LINKER_FLAGS} -fuse-ld=lld")
include(${CMAKE_CURRENT_LIST_DIR}/riscv64-common.cmake)

set(CMAKE_C_COMPILER clang)
