#pragma once

#define ALLOW_UNUSED  __attribute__((unused))
#define ALWAYS_INLINE inline __attribute__((always_inline))
#define EXPORT        __attribute__((__visibility__("default")))

#define THREAD_LOCAL _Thread_local
