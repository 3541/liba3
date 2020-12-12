#pragma once

#ifdef _MSC_VER
#define ALLOW_UNUSED
#define ALWAYS_INLINE __forceinline
#define EXPORT        __declspec(dllexport)
#else
#define ALLOW_UNUSED  __attribute__((unused))
#define ALWAYS_INLINE inline __attribute__((always_inline))
#define EXPORT        __attribute__((__visibility__("default")))
#endif

#define THREAD_LOCAL _Thread_local
