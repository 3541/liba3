cmake_minimum_required(VERSION 3.10...3.17)

project(
  liba3
  VERSION 0.1.0
  LANGUAGES C CXX
)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/boilerplate/cmake" ${CMAKE_MODULE_PATH})

set(default_build_type "Release")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Unspecified build type. Using ${default_build_type}.")
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "RelWithDebInfo")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CheckIPOSupported)
check_ipo_supported(RESULT lto_support)

add_compile_options(-Wall -Wextra -pedantic)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

add_library(a3
  src/buffer.c
  src/log.c
  src/pool.c
  src/str.c
)
target_include_directories(a3 PUBLIC src/include)

include(AddWarnings)
target_add_extra_warnings(a3)
target_add_warning(a3 "no-pointer-arith" FALSE)
target_add_warnings(a3 "no-cast-qual;no-override-init" TRUE)
set_target_properties(a3 PROPERTIES
  C_STANDARD 11
  C_EXTENSIONS OFF
)
target_compile_definitions(a3 PRIVATE _XOPEN_SOURCE)
if (lto_support)
  set_target_properties(a3 PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
    INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE
    )
endif()

add_subdirectory(extern/highwayhash EXCLUDE_FROM_ALL)

add_library(a3hash INTERFACE)
target_include_directories(a3hash INTERFACE src/include)
target_link_libraries(a3hash INTERFACE highwayhash)

# Testing
include(CTest)
find_package(GTest)

if (GTest_FOUND)
  set(GTEST_LIB GTest::gtest_main)
else()
  add_subdirectory(extern/googletest EXCLUDE_FROM_ALL)
  set(GTEST_LIB gtest_main)
endif()

add_executable(a3_test test/log.cc test/str.cc)
target_compile_features(a3_test PRIVATE cxx_std_20)
set_target_properties(a3_test PROPERTIES CXX_EXTENSIONS OFF)
target_add_extra_warnings(a3_test)
if (NOT MSVC)
  target_add_warning(a3_test "no-c99-extensions" FALSE)
endif()
target_link_libraries(a3_test PRIVATE a3 ${GTEST_LIB})
gtest_discover_tests(a3_test)
