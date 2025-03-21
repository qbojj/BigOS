project(example LANGUAGES C ASM)
add_executable(example)

file(GLOB SOURCES CONFIGURE_DEPENDS *.c)
file(GLOB HEADERS CONFIGURE_DEPENDS *.h)
file(GLOB AS_SOURCES CONFIGURE_DEPENDS *.s)

target_sources(example
  PRIVATE
    ${SOURCES}
    ${AS_SOURCES}
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS .
    FILES ${HEADERS}
  )

target_link_libraries(example PRIVATE Debug)
target_link_options(example PRIVATE -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld)
target_compile_definitions(example PRIVATE __DEBUG__=1)

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
)
