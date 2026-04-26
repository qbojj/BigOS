SETUP_EXECUTABLE(example_sbi)

target_link_libraries(example_sbi PRIVATE startup stdbigos)

<<<<<<< HEAD
target_link_options(example_sbi PRIVATE -static -T ${LINKER_SCRIPT})
set_target_properties(example_sbi
    PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

target_link_libraries(example_sbi PRIVATE stdbigos)

COMPILE_BINARY(example_sbi)
=======
>>>>>>> origin/main
ADD_QEMU_TARGET(example_sbi)
