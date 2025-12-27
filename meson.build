project(
  'liba3',
  ['c', 'cpp'],
  version: '0.6.0',
  default_options: [
    'c_std=c11',

    meson.version().endswith('.99') ? 'cpp_std=c++2a' : 'cpp_std=c++23,c++20,c++17',
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

a3_flags_wanted = {
  'gcc': ['-fstack-protector', '-fstack-clash-protection'],
  'msvc': ['/GS', '/Zc:__cplusplus']
}
add_project_arguments(c.get_supported_arguments(a3_flags_wanted[c_arg_syntax]), language: 'c')
add_project_link_arguments(c.get_supported_link_arguments(a3_flags_wanted[c_arg_syntax]),
                           language: 'c')
add_project_arguments(cxx.get_supported_arguments(a3_flags_wanted[cxx_arg_syntax]), language: 'cpp')
add_project_link_arguments(cxx.get_supported_link_arguments(a3_flags_wanted[cxx_arg_syntax]),
                           language: 'cpp')

if get_option('buildtype') != 'debug'
  add_project_arguments(c.get_supported_arguments(['-U_FORTIFY_SOURCE', '-D_FORTIFY_SOURCE=3']), language: 'c')
  add_project_arguments(cxx.get_supported_arguments(['-U_FORTIFY_SOURCE', '-D_FORTIFY_SOURCE=3']), language: 'cpp')
endif

if host_machine.system() == 'windows'
  add_project_arguments('-D_HAS_EXCEPTIONS=0', language: 'cpp')
endif

subdir('third_party')
subdir('a3')
subdir('test')
subdir('doc')
