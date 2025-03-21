add_library(Debug)

file(GLOB SOURCES CONFIGURE_DEPENDS src/*.c)
file(GLOB HEADERS CONFIGURE_DEPENDS include/*.h)
file(GLOB AS_SOURCES CONFIGURE_DEPENDS src/*.s)

target_sources(Debug
  PRIVATE
    ${SOURCES}
    ${AS_SOURCES}
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES ${HEADERS}
  )

target_include_directories(Debug PUBLIC include)
