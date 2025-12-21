project(
  'liba3',
  ['c', 'cpp'],
  version: '0.5.0',
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

subdir('a3')
subdir('test')
subdir('doc')
