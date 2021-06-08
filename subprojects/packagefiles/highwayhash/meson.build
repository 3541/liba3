project(
  'highwayhash',
  ['c', 'cpp'],
  default_options: [
    'c_std=c99',

    'cpp_std=c++11',
    'cpp_rtti=false',
    'cpp_eh=none',

    'warning_level=2',
    'b_ndebug=if-release'
  ]
)

highwayhash_c_include = include_directories('c')
highwayhash_c_lib = library(
  'highwayhash_c',
  'c/highwayhash.c',
  include_directories: highwayhash_c_include,
  build_by_default: false
)

highwayhash_c_dep = declare_dependency(
  link_with: highwayhash_c_lib,
  include_directories: highwayhash_c_include
)
