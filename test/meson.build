if not meson.is_subproject()
  gmock_main = dependency('gmock_main', fallback: ['gtest', 'gmock_main_dep'])

  a3_test_src = files(
    [
      'buf.cc',
      'cache.cc',
      'ht.cc',
      'll.cc',
      'log.cc',
      'option.cc',
      'pool.cc',
      'rc.cc',
      'sll.cc',
      'str.cc',
      'try.cc',
      'vec.cc',
    ]
  )

  a3_test_args = ['-DGTEST_HAS_EXCEPTIONS=0']

  if host_machine.system() == 'windows'
    a3_test_args += '-D_CRT_SECURE_NO_WARNINGS'
  endif

  a3_test_args += cxx.get_supported_arguments(['-Wno-keyword-macro', '-Wno-pedantic', '-wd4324'])

  if (cxx.get_id() == 'gcc' or cxx.get_id() == 'clang') and cxx.version().split('.')[0].to_int() < 5
    warning(cxx.get_id() + ' ' + cxx.version() + ' not supported by gtest. Tests disabled.')
  else
    a3_test = executable(
      'a3_test',
      a3_test_src,
      cpp_args: a3_test_args,
      dependencies: [gmock_main, a3],
      build_by_default: false
    )

    test('a3_gtest', a3_test, protocol: 'gtest')
  endif
endif
