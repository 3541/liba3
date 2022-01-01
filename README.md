# liba3

![Build and test](https://github.com/3541/liba3/workflows/Build%20and%20test/badge.svg)

My personal C library. All the little things I keep rewriting for every
nontrivial C project. This was largely born out of frustration while working on
[Short Circuit](https://github.com/3541/short-circuit).

Note: This library is absolutely neither stable nor mature, and not ready for any kind of external
use at this time. Show-stopping bugs probably exist, and the API may change drastically without any
notice.

## Provides
- Growable byte buffer.
- Hash table (open addressing, Robin Hood).
- Cache.
- Intrusive singly and doubly-linked lists.
- Priority queue (binary heap, WIP).
- Logging (to arbitrary `FILE*` outputs).
- Pool allocator.
- Nicer strings.
- Refcounting.

## Features
- Simple and lightweight — <2 kLOC. Less, without the optional `a3_hash` component.
- Type-generic data structures — (Ab)uses preprocessor macros to imitate C++ templates.
- C++ friendly — All headers are compatible, and convenient C++ wrappers are provided where
  appropriate (C++ is not required, however).

## Building and Usage
Build dependencies:
- A C compiler supporting C11 or later.
- Meson 0.55 or later (note: 0.56 or later is required on Windows).
- Ninja.

Test suite dependencies:
- A C++ compiler supporting C++14.

To build, ensure all submodules are in place (`git submodule update --init --recursive`), and run
`meson setup <BUILDDIR>` to set up the build system in `BUILDDIR`. A script which generates a set
of build directories for various configurations is also provided (`./configure`) for convenience's
sake. Then, run `meson compile -C <BUILDDIR>` to build.

To run the test suite, simply run `meson test -C <BUILDDIR>`.

Meson projects should easily be able to hook into the build system using `subproject`. The Meson
build system provides dependencies `a3_dep` and `a3_hash_dep`.

Projects using other build systems can invoke Meson directly and depend on the library produced (or
they can simply build it themselves — take look at `meson.build` for inspiration).


## Notes
Most objects (buffer, hash table, linked list, etc...) provide the following functions to do with their lifecycle:

- `void X_init(X*, [...])` initializes a new object.
- `X* X_new([...])` allocates and initializes (by calling `X_init`) a new object.
- `void X_destroy(X*)` deinitializes an object and frees its owned memory.
- `void X_free(X*)` deinitializes (by calling `X_destroy`) and frees an object.

This vaguely mirrors the C++ object lifecycle, where `X_new` and `X_free` mimic `new` and `delete`, and `X_init` and `X_destroy` are the constructor and destructor.

For objects with value semantics (`A3String`), there is no `init` or `destroy`, and `new` returns a
value, not a pointer. `free` still takes a pointer so that it can invalidate internal pointers.

## Licensing

`liba3` is licensed under the 3-clause BSD license, the terms of which are
described [here](https://github.com/3541/liba3/blob/trunk/LICENSE).

`liba3` uses the following third-party projects:

### HighayHash
The project links with [HighwayHash](https://github.com/google/highwayhash),
which is licensed under the [Apache
license](https://github.com/google/highwayhash/blob/master/LICENSE).

### Google Test
The test framework uses [Google Test](https://github.com/google/googletest),
which is licensed under the [3-clause BSD
license](https://github.com/google/googletest/blob/master/LICENSE).
