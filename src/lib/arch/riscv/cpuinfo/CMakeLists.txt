SETUP_LIBRARY(riscv_cpuinfo)

target_link_libraries(riscv_cpuinfo
  PUBLIC
    device_tree_access
    stdbigos
)
