project(
  'liba3',
  ['c', 'cpp'],
  version: '0.1.0-alpha',
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

a3_c_flags = ['-D__STDC_WANT_LIB_EXT1__=1', '-D_XOPEN_SOURCE=600']
a3_cxx_flags = ['-D__STDC_WANT_LIB_EXT1__=1', '-D_XOPEN_SOURCE=600']

if host_machine.system() == 'openbsd'
  a3_c_flags += '-D_BSD_SOURCE'
  a3_cxx_flags += '-D_BSD_SOURCE'
elif host_machine.system() == 'linux'
  a3_c_flags += '-D_DEFAULT_SOURCE'
  a3_cxx_flags += '-D_DEFAULT_SOURCE'
elif host_machine.system() == 'windows'
  a3_c_flags += ['-DWIN32_LEAN_AND_MEAN', '-D_CRT_SECURE_NO_WARNINGS']
  a3_cxx_flags += ['-DWIN32_LEAN_AND_MEAN', '-D_CRT_SECURE_NO_WARNINGS']
  add_project_arguments('-D_HAS_EXCEPTIONS=0', language: 'cpp')
endif

a3_flags_wanted = {
  'gcc': ['-fstack-protector', '-fstack-clash-protection'],
  'msvc': ['/GS']
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
    '-Weffc++', '-Wno-pedantic'
  ],

  'msvc': [
    '-wd5105'
  ]
}

a3_c_flags += c.get_supported_arguments(a3_warnings_wanted[c_arg_syntax])
a3_cxx_flags += cxx.get_supported_arguments(a3_warnings_wanted[cxx_arg_syntax])

if (c.get_id() != 'gcc' or not c.version().startswith('9')) and (cxx.get_id() != 'gcc' or not cxx.version().startswith('9'))
  # -Wconversion is too aggressive on GCC <= 9.
  a3_c_flags += c.get_supported_arguments(['-Wconversion'])
  a3_cxx_flags += cxx.get_supported_arguments(['-Wconversion'])
endif

a3_features_wanted = {
  '_Thread_local': '''
    _Thread_local int x = 123;
    int main(void) {
      return 0;
    }''',
  '_Generic': '''
    #define GEN_MACRO(X) _Generic((X), int: "int", float: "float")
    int main(void) {
      int x;
      const char* res = GEN_MACRO(x);
      return 0;
    }'''
}

a3_features_c = configuration_data()
a3_features_cxx = configuration_data()

foreach name, code : a3_features_wanted
  if c.compiles(code, name: 'CC supports ' + name)
    a3_features_c.set('a3_HAVE_' + name, true)
  endif

  if cxx.compiles(code, name: 'CXX supports ' + name)
    a3_features_cxx.set('a3_HAVE_' + name, true)
  endif
endforeach

if c.has_header_symbol('sys/types.h', 'ssize_t')
  a3_features_c.set('a3_HAVE_ssize_t', true)
  a3_features_cxx.set('a3_HAVE_ssize_t', true)
endif

a3_functions_wanted = ['memset_s', 'explicit_bzero']

foreach function : a3_functions_wanted
  if c.has_function(function)
    a3_features_c.set('a3_HAVE_' + function, true)
    a3_features_cxx.set('a3_HAVE_' + function, true)
  endif
endforeach

if host_machine.system() == 'windows'
  if c.has_header_symbol('Windows.h', 'SecureZeroMemory')
    a3_features_c.set('a3_HAVE_SecureZeroMemory', true)
    a3_features_cxx.set('a3_HAVE_SecureZeroMemory', true)
  endif
endif

a3_features_c = configure_file(output: 'features_c.h', configuration: a3_features_c)
a3_features_cxx = configure_file(output: 'features_cc.hh', configuration: a3_features_cxx)

a3_include = include_directories(['src/include', '.'])
a3_src = files(
  [
    'src/buffer.c',
    'src/log.c',
    'src/pool.c',
    'src/str.c'
  ]
)

a3_lib = library(
  'a3',
  a3_src,
  include_directories: a3_include,
  dependencies: libm,
  c_args: a3_c_flags,
  cpp_args: a3_cxx_flags,
  gnu_symbol_visibility: 'hidden',
  build_by_default: true
)

a3_compile_args = ['-D__STDC_WANT_LIB_EXT1__=1']
if host_machine.system() == 'windows'
  a3_compile_args += '-D_CRT_SECURE_NO_WARNINGS'
endif

a3 = declare_dependency(
  link_with: a3_lib,
  include_directories: a3_include,
  compile_args: a3_compile_args
)
a3_dep = a3

highwayhash_c = dependency('highwayhash_c', fallback: ['highwayhash', 'highwayhash_c_dep'])
a3_hash = declare_dependency(dependencies: [a3, highwayhash_c], include_directories: a3_include)
a3_hash_dep = a3_hash

subdir('test')
subdir('doc')
