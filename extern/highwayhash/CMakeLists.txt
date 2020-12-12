project(
  highwayhash
  VERSION 1.0.0
  LANGUAGES C
)

set(HH_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/highwayhash)
set(HH_INST_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(HH_HEADER ${HH_INST_DIR}/include/highwayhash/highwayhash.h)

add_custom_command(
  OUTPUT ${HH_HEADER}
  COMMAND ${CMAKE_COMMAND} -E copy ${HH_SRC_DIR}/c/highwayhash.h ${HH_HEADER}
)

add_library(highwayhash EXCLUDE_FROM_ALL ${HH_SRC_DIR}/c/highwayhash.c ${HH_HEADER})
target_include_directories(highwayhash PUBLIC ${HH_INST_DIR}/include)
