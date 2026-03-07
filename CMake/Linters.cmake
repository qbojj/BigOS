set(BIGOS_USE_CLANG_TIDY OFF CACHE BOOL "Check using clang-tidy during build")
set(BIGOS_USE_INCLUDE_WHAT_YOU_USE OFF CACHE BOOL "Check using include-what-you-use during build")

find_program(CLANG_TIDY clang-tidy)
find_program(INCLUDE_WHAT_YOU_USE include-what-you-use)

set(BIGOS_IWYU_FLAGS -Xiwyu "--mapping_file=${CMAKE_SOURCE_DIR}/CMake/iwyu.imp")

if (BIGOS_USE_CLANG_TIDY)
    set(CMAKE_C_CLANG_TIDY ${CLANG_TIDY})
endif()

if (BIGOS_USE_INCLUDE_WHAT_YOU_USE)
    set(CMAKE_C_INCLUDE_WHAT_YOU_USE ${INCLUDE_WHAT_YOU_USE} ${BIGOS_IWYU_FLAGS})
endif()
