project(
  'liba3',
  ['c'],
  version: '0.1.0',
  default_options: [
    'c_std=c18',
    'warning_level=2',
    'werror=true',
    'buildtype=debug',
    'b_ndebug=if-release'
  ]
)

c_compiler = meson.get_compiler('c')

add_project_arguments('-D_XOPEN_SOURCE', language: 'c')

# Extra useful warnings.
extra_c_warnings = [
  'bad-function-cast',
  'disabled-optimization',
  'duplicated-branches',
  'duplicated-cond',
  'float-equal',
  'format-nonliteral',
  'format-security',
  'format=2',
  'implicit',
  'inline',
  'logical-op',
  'missing-declarations',
  'missing-prototypes',
  'nested-externs',
  'null-dereference',
  'shadow',
  'strict-prototypes',
  'undef'
]

foreach warning : extra_c_warnings
  warning = '-W' + warning
  if c_compiler.has_argument(warning)
    add_project_arguments(warning, language: 'c')
  endif
endforeach

headers = include_directories('src/include')

src = files([
             'src/buffer.c',
             'src/log.c',
             'src/str.c'
           ])

liba3 = library(
  'a3',
  src,
  include_directories: headers,
  install: true,
  build_by_default: true
)

liba3_dep = declare_dependency(include_directories: headers, link_with: liba3)
