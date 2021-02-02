cmake_minimum_required(VERSION 3.10...3.17)

project(
  liba3
  VERSION 0.1.0
  LANGUAGES C CXX
)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/boilerplate/cmake" ${CMAKE_MODULE_PATH})

include(FixDefaultFlags)
fix_default_flags()

set(default_build_type "Release")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Unspecified build type. Using ${default_build_type}.")
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "RelWithDebInfo")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CheckIPOSupported)
check_ipo_supported(RESULT lto_support)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

include(ClangTidy)
include(PlatformChecks)
include(IWYU)
add_library(a3
  src/buffer.c
  src/log.c
  src/pool.c
  src/str.c

  src/include/a3/buffer.h
  src/include/a3/cpp.h
  src/include/a3/forward.h
  src/include/a3/ht.h
  src/include/a3/ll.h
  src/include/a3/log.h
  src/include/a3/platform/types.h
  src/include/a3/platform/types_private.h
  src/include/a3/platform/util.h
  src/include/a3/pool.h
  src/include/a3/pq.h
  src/include/a3/sll.h
  src/include/a3/str.h
  src/include/a3/util.h
)
target_include_directories(a3 PUBLIC src/include)
target_link_math(a3)

include(AddWarnings)
target_add_extra_warnings(a3 TRUE)
target_platform_checks(a3)
if (NOT MSVC)
  target_add_warning(a3 "no-pointer-arith" FALSE)
  target_add_warning(a3 "no-cast-qual" TRUE)
  target_add_warning_c(a3 "no-override-init" TRUE)
endif()
set_target_properties(a3 PROPERTIES
  C_STANDARD 11
  C_STANDARD_REQUIRED TRUE
  C_EXTENSIONS OFF
)
target_compile_definitions(a3 PRIVATE _XOPEN_SOURCE=600)
if (${CMAKE_SYSTEM_NAME} STREQUAL OpenBSD)
  target_compile_definitions(a3 PUBLIC _BSD_SOURCE)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL Linux)
  target_compile_definitions(a3 PUBLIC _DEFAULT_SOURCE)
endif()
if (lto_support)
  set_target_properties(a3 PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
    INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE
  )
endif()
target_add_iwyu(a3)
target_add_clang_tidy(a3)

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
  set(INSTALL_GTEST FALSE CACHE BOOL "Don't install GTest.")
  add_subdirectory(extern/googletest EXCLUDE_FROM_ALL)
  set(GTEST_LIB gtest_main)

  if (WIN32 AND NOT MSVC)
    target_add_warning(gtest "no-language-extension-token" FALSE)
  endif()
endif()

include(AddFlags)
include(DisableExceptions)
include(DisableRTTI)
include(Sanitize)
include(SetCXX20)

add_executable(a3_test EXCLUDE_FROM_ALL
  test/buf.cc
  test/cache.cc
  test/ht.cc
  test/ll.cc
  test/log.cc
  test/pool.cc
  test/sll.cc
  test/str.cc
)
target_disable_exceptions(a3_test)
target_disable_rtti(a3_test)
target_set_cxx_std_20(a3_test)
set_target_properties(a3_test PROPERTIES
  CXX_EXTENSIONS OFF
  C_EXTENSIONS OFF
)
target_add_sanitizers(a3_test TRUE)
target_add_extra_warnings(a3_test FALSE)
if (NOT MSVC)
  target_add_warnings(a3_test "no-c99-extensions" FALSE)
endif()

target_link_libraries(a3_test PRIVATE a3hash)
target_link_libraries(a3_test PRIVATE a3 ${GTEST_LIB})
gtest_discover_tests(a3_test)

add_custom_target(a3_check COMMAND ${CMAKE_CTEST_COMMAND}
  DEPENDS a3_test
)
