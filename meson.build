project(
  'liba3',
  ['c', 'cpp'],
  version: '0.4.2',
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
if build_machine.system() == 'linux' and c.has_argument('-Werror')
  trivial = '''
  #include <features.h>
  int main(void) {}
  '''
  if get_option('buildtype') == 'debug' and not c.compiles(
    trivial,
    args: '-Werror',
    name: 'Trivial program'
  )
    if c.compiles(
      trivial,
      args: ['-Werror', '-U_FORTIFY_SOURCE'],
      name: 'Trivial program with -U_FORTIFY_SOURCE'
    )
      add_global_arguments('-U_FORTIFY_SOURCE', language: 'c')
      add_global_arguments('-U_FORTIFY_SOURCE', language: 'cpp')
    else
      error('Cannot compile a trivial program.')
    endif
  endif
endif

subdir('src')
subdir('test')
subdir('doc')
