gtest_main = dependency('gtest_main', fallback: ['gtest', 'gtest_main_dep'])

a3_test_src = files(
  [
    'buf.cc',
    'cache.cc',
    'ht.cc',
    'll.cc',
    'log.cc',
    'pool.cc',
    'rc.cc',
    'sll.cc',
    'str.cc'
  ]
)

a3_test = executable(
  'a3_test',
  a3_test_src,
  dependencies: [gtest_main, a3, a3_hash],
  build_by_default: false
)

test('a3_gtest', a3_test, protocol: 'gtest')
