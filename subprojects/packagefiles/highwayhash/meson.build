project(
  'highwayhash',
  ['c', 'cpp'],
  default_options: [
    'c_std=c99',

    'cpp_std=c++11',
    'cpp_rtti=false',
    'cpp_eh=none',

    'warning_level=3',
    'b_ndebug=if-release'
  ]
)

c = meson.get_compiler('c')

highwayhash_c_args = []
if host_machine.system() == 'windows'
  # Disable warning for adding size_t to int.
  highwayhash_c_args += c.get_supported_arguments('-wd4267')
endif

highwayhash_c_include = include_directories('c')
highwayhash_c_lib = library(
  'highwayhash_c',
  'c/highwayhash.c',
  include_directories: highwayhash_c_include,
  c_args: highwayhash_c_args,
  build_by_default: false
)

highwayhash_c_dep = declare_dependency(
  link_with: highwayhash_c_lib,
  include_directories: highwayhash_c_include
)
