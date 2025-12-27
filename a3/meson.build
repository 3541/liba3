subdir('shim')

c = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')

c_arg_syntax = c.get_argument_syntax()
cxx_arg_syntax = cxx.get_argument_syntax()

libm = c.find_library('m', required: false)
libatomic = c.find_library('atomic', required: false)

a3_common_flags = []
a3_c_flags = []
a3_cxx_flags = []

if host_machine.system() == 'linux'
  a3_common_flags += '-D_XOPEN_SOURCE=600'
elif host_machine.system() == 'openbsd'
  a3_common_flags += '-D_BSD_SOURCE'
elif host_machine.system() == 'sunos'
  a3_common_flags += '-D__EXTENSIONS__'
elif host_machine.system() == 'windows'
  a3_common_flags += ['-DWIN32_LEAN_AND_MEAN', '-D_CRT_SECURE_NO_WARNINGS']
endif

if get_option('profile')
  a3_common_flags += '-DPROFILE'
endif

a3_warnings_wanted = {
  'gcc': [
    '-Wdisabled-optimization', '-Wduplicated-branches', '-Wduplicated-cond', '-Wfloat-equal',
    '-Wformat-nonliteral', '-Wformat-security', '-Wlogical-op', '-Wmissing-declarations',
    '-Wmissing-include-dirs', '-Wnull-dereference', '-Wpacked', '-Wshadow', '-Wstack-protector',
    '-Wundef', '-Wcast-align', '-Wbad-function-cast', '-Wimplicit', '-Wmissing-prototypes',
    '-Wnested-externs', '-Wstrict-prototypes', '-Wctor-dtor-privacy', '-Wdelete-non-virtual-dtor',
    '-Wconversion', '-Weffc++', '-Wpointer-arith', '-Wno-pedantic'
  ],

  'msvc': ['-wd5105', '-wd4189']
}

a3_c_flags += c.get_supported_arguments(a3_warnings_wanted[c_arg_syntax])
a3_cxx_flags += cxx.get_supported_arguments(a3_warnings_wanted[cxx_arg_syntax])

a3_include = include_directories(['..'])
a3_src = files(
  [
    'buffer.c',
    'log.c',
    'pool.c',
    'str.c',
    'vec.c',
  ]
)
a3_src += a3_shim_src

a3_lib = library(
  'a3',
  a3_src,
  include_directories: a3_include,
  dependencies: [libm, libatomic] + a3_shim_deps,
  link_with: [highwayhash],
  c_args: a3_c_flags + a3_common_flags,
  cpp_args: a3_cxx_flags + a3_common_flags,
  gnu_symbol_visibility: 'hidden',
  build_by_default: true,
  install: true
)
pkg = import('pkgconfig')
pkg.generate(a3_lib)

a3 = declare_dependency(
  link_with: a3_lib,
  include_directories: a3_include,
)
a3_dep = a3
