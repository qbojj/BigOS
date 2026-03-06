SETUP_EXECUTABLE(kernel)

target_compile_definitions(kernel PRIVATE $<IF:$<CONFIG:DEBUG>,__DEBUG__ __LOGLVL__=3, __LOGLVL__=1>)
target_link_libraries(kernel PRIVATE Debug stdbigos)

target_link_options(kernel PRIVATE -static-pie -e kinit)
target_compile_options(kernel PRIVATE -fPIE -static)

ADD_QEMU_TARGET(kernel)
