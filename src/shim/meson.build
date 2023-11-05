c = meson.get_compiler('c')

a3_shim_src = []

foreach s : [[['memmem', 'string.h']],
             [['strncasecmp', 'strings.h'], ['_strnicmp', 'string.h']],
             [['aligned_alloc', 'stdlib.h'], ['_aligned_malloc', 'malloc.h']]]
  found = ''
  foreach f : s
    if c.has_function(f[0], prefix: '#include <' + f[1] + '>', args: '-D_GNU_SOURCE')
      found = f[0]
      break
    endif
  endforeach

  if found != ''
    if s.length() == 1
      a3_shim_src += files(s[0][0] / 'present.c')
    else
      a3_shim_src += files(s[0][0] / found + '.c')
    endif
  else
    a3_shim_src += files(s[0][0] / 'missing.c')
  endif
endforeach

if c.has_function('strerror_r')
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
    message('strerror_r has the GNU signature.')
    a3_shim_src += files('strerror/gnu_strerror_r.c')
  else
    message('strerror_r has the XSI signature.')
    a3_shim_src += files('strerror/xsi_strerror_r.c')
  endif
elif c.has_function('strerror_s')
  a3_shim_src += files('strerror/strerror_s.c')
endif

if c.check_header('stdatomic.h') or c.has_header_symbol('stddef.h', '__atomic_compare_exchange')
  # Neither of these requires a C file.
elif c.has_header_symbol('windows.h', 'InterlockedCompareExchange')
  a3_shim_src += files('atomic/msvc.c')
else
  error('No atomic primitives found.')
endif
