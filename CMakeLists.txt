cmake_minimum_required(VERSION 3.28)

project(BigOS LANGUAGES C ASM)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(src)

install(TARGETS example)
