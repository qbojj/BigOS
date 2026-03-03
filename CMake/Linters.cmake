set(BIGOS_USE_CLANG_TIDY OFF CACHE BOOL "Check using clang-tidy during build")
set(BIGOS_USE_INCLUDE_WHAT_YOU_USE OFF CACHE BOOL "Check using include-what-you-use during build")

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    execute_process(
        COMMAND brew --prefix llvm
        OUTPUT_VARIABLE LLVM_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

find_program(CLANG_TIDY clang-tidy HINTS "${LLVM_PREFIX}/bin")
find_program(INCLUDE_WHAT_YOU_USE include-what-you-use HINTS "${LLVM_PREFIX}/bin")

if (BIGOS_USE_CLANG_TIDY)
    set(CMAKE_C_CLANG_TIDY ${CLANG_TIDY})
endif()

if (BIGOS_USE_INCLUDE_WHAT_YOU_USE)
    set(CMAKE_C_INCLUDE_WHAT_YOU_USE ${INCLUDE_WHAT_YOU_USE})
endif()
