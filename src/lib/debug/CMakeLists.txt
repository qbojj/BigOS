project(Debug LANGUAGES C)
add_library(Debug)

file(GLOB SOURCES CONFIGURE_DEPENDS src/*.c)
file(GLOB AS_SOURCES CONFIGURE_DEPENDS src/*.s)
file(GLOB HEADERS CONFIGURE_DEPENDS include/*.h)

target_sources(Debug
  PRIVATE
    ${SOURCES}
    ${AS_SOURCES}
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES ${HEADERS}
  )
