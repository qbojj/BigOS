SETUP_EXECUTABLE(example_sbi)

target_link_libraries(example_sbi PRIVATE startup stdbigos)

ADD_QEMU_TARGET(example_sbi)
