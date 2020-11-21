project(
  'liba3',
  ['c'],
  version: '0.1.0',
  default_options: [
    'c_std=c18',
    'warning_level=2',
    'werror=true'
  ]
)

headers = include_directories('src/include')

liba3_dep = declare_dependency(include_directories: headers)
