highwayhash_c_include = include_directories('highwayhash/c')
highwayhash_c_lib = library(
  'highwayhash_c',
  'highwayhash/c/highwayhash.c',
  include_directories: highwayhash_c_include,
  build_by_default: false
)

highwayhash_c = declare_dependency(
  link_with: highwayhash_c_lib,
  include_directories: highwayhash_c_include
)
