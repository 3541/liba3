project(
  'liba3',
  ['c', 'cpp'],
  version: '0.3.13',
  default_options: [
    'c_std=c11',

    'cpp_std=c++14',
    'cpp_rtti=false',
    'cpp_eh=none',

    'warning_level=3',
    'buildtype=debug',
    'b_ndebug=if-release',
    'default_library=static'
  ]
)

c = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')

c_arg_syntax = c.get_argument_syntax()
cxx_arg_syntax = cxx.get_argument_syntax()

libm = c.find_library('m', required: false)

a3_common_flags = []
a3_c_flags = []
a3_cxx_flags = []

if host_machine.system() == 'openbsd'
  a3_common_flags += '-D_BSD_SOURCE'
elif host_machine.system() == 'linux'
  a3_common_flags += '-D_XOPEN_SOURCE=600'
elif host_machine.system() == 'windows'
  a3_common_flags += ['-DWIN32_LEAN_AND_MEAN', '-D_CRT_SECURE_NO_WARNINGS']
  add_project_arguments('-D_HAS_EXCEPTIONS=0', language: 'cpp')
endif

if get_option('profile')
  a3_common_flags += '-DPROFILE'
endif

a3_flags_wanted = {
  'gcc': ['-fstack-protector', '-fstack-clash-protection'],
  'msvc': ['/GS', '/Zc:__cplusplus']
}
add_project_arguments(c.get_supported_arguments(a3_flags_wanted[c_arg_syntax]), language: 'c')
add_project_arguments(cxx.get_supported_arguments(a3_flags_wanted[cxx_arg_syntax]), language: 'cpp')

a3_warnings_wanted = {
  'gcc': [
    '-Wdisabled-optimization', '-Wduplicated-branches', '-Wduplicated-cond', '-Wfloat-equal',
    '-Wformat-nonliteral', '-Wformat-security', '-Wlogical-op', '-Wmissing-declarations',
    '-Wmissing-include-dirs', '-Wnull-dereference', '-Wpacked', '-Wshadow', '-Wstack-protector',
    '-Wundef', '-Wcast-align', '-Wbad-function-cast', '-Wimplicit', '-Wmissing-prototypes',
    '-Wnested-externs', '-Wstrict-prototypes', '-Wctor-dtor-privacy', '-Wdelete-non-virtual-dtor',
    '-Wconversion', '-Weffc++', '-Wno-pedantic'
  ],

  'msvc': [
    '-wd5105'
  ]
}

a3_c_flags += c.get_supported_arguments(a3_warnings_wanted[c_arg_syntax])
a3_cxx_flags += cxx.get_supported_arguments(a3_warnings_wanted[cxx_arg_syntax])

if c.has_function('memmem', args: ['-D_GNU_SOURCE', '-D_DARWIN_C_SOURCE=__DARWIN_C_FULL'])
  a3_c_flags += '-DA3_HAVE_MEMMEM'
endif

if c.has_function('strerror_r')
  a3_c_flags += '-DA3_HAVE_STRERROR_R'

  # If the return value of strerror_r can be dereferenced and assigned to a char, it is most likely
  # the _GNU_SOURCE version. The XSI version returns int.
  strerror_test = '''
  #include <string.h>
  #include <errno.h>

  int main(void) {
      char buf[32] = { '\0' };
      char c = *strerror_r(EAGAIN, buf, sizeof(buf));
      return 0;
  }
  '''
  if c.compiles(strerror_test)
    a3_c_flags += '-DA3_HAVE_GNU_STRERROR_R'
  endif
elif c.has_function('strerror_s')
  a3_c_flags += '-DA3_HAVE_STRERROR_S'
endif

if c.has_function('strncasecmp')
  a3_c_flags += '-DA3_HAVE_STRNCASECMP'
elif c.has_function('_strnicmp')
  a3_c_flags += '-DA3_HAVE_STRNICMP'
endif

a3_include = include_directories(['src/include', '.'])
a3_src = files(
  [
    'src/buffer.c',
    'src/ll.c',
    'src/log.c',
    'src/pool.c',
    'src/sll.c',
    'src/str.c',
    'src/shim/memmem.c',
    'src/shim/strerror.c',
    'src/shim/strncasecmp.c',
  ]
)

a3_lib = library(
  'a3',
  a3_src,
  include_directories: a3_include,
  dependencies: libm,
  c_args: a3_c_flags + a3_common_flags,
  cpp_args: a3_cxx_flags + a3_common_flags,
  gnu_symbol_visibility: 'hidden',
  build_by_default: true,
  install: true
)
install_subdir('src/include', install_dir: 'include', strip_directory: true)
pkg = import('pkgconfig')
pkg.generate(a3_lib)

a3 = declare_dependency(
  link_with: a3_lib,
  include_directories: include_directories(['src/include']),
)
a3_dep = a3

highwayhash_c = dependency('highwayhash_c', fallback: ['highwayhash', 'highwayhash_c_dep'])
a3_hash = declare_dependency(
  dependencies: [a3, highwayhash_c],
  include_directories: include_directories(['src/include'])
)
a3_hash_dep = a3_hash

subdir('test')
subdir('doc')
