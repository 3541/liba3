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
    'result.cc',
    'sll.cc',
    'str.cc',
    'try.cc'
  ]
)

a3_test_args = ['-DGTEST_HAS_EXCEPTIONS=0']
a3_test_cxx_args = a3_test_args

a3_test_cxx_args += cxx.get_supported_arguments(['-Wno-keyword-macro', '-Wno-pedantic'])

a3_test = executable(
  'a3_test',
  a3_test_src,
  cpp_args: a3_test_cxx_args,
  dependencies: [gmock_main, a3, a3_hash],
  build_by_default: false
)

test('a3_gtest', a3_test, protocol: 'gtest')
