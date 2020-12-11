project(
  highwayhash
  VERSION 1.0.0
  LANGUAGES C CXX
)

find_program(MAKE_EXE NAMES gmake nmake make)

set(HH_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/highwayhash)
set(HH_INST_DIR ${CMAKE_CURRENT_BINARY_DIR}/prefix)
set(HH_LIB ${HH_INST_DIR}/lib/libhighwayhash.a)

file(GLOB HH_SRC "${HH_SRC_DIR}/highwayhash/*.cc" "${HH_SRC_DIR}/highwayhash/*.h")

add_custom_command(
  OUTPUT ${HH_LIB}
  WORKING_DIRECTORY ${HH_SRC_DIR}
  COMMAND ${MAKE_EXE} lib/libhighwayhash.a
  COMMAND ${MAKE_EXE} PREFIX=${HH_INST_DIR} install
  DEPENDS ${HH_SRC}
)
add_custom_target(highwayhash_dep DEPENDS ${HH_LIB})
add_library(highwayhash INTERFACE)
target_link_libraries(highwayhash INTERFACE ${HH_LIB})
target_include_directories(highwayhash INTERFACE ${HH_INST_DIR}/include)
add_dependencies(highwayhash highwayhash_dep)
