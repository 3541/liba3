# liba3

My personal C library. All the little things I keep rewriting for every
nontrivial C project. This was largely born out of frustration while working on
[Short Circuit](https://github.com/3541/short-circuit).

## Provides
- Growable buffer.
- Hash table (open addressing, Robin Hood).
- Intrusive doubly-linked list.
- Priority queue (binary heap).
- Logging (to arbitrary `FILE*` outputs).
- Pool allocator.
- Safer strings.

## Features
- Simple and lightweight — approx. 1 kLOC. Less, without `a3_hash`.
- Type-generic data structures — (ab)uses preprocessor macros to imitate C++ templates.
- C++ compatible — can be freely used in mixed projects.

## Building and Usage
Dependencies:
- A C compiler supporting C11 or later.
- CMake 3.10 or later.

Test suite dependencies:
- A C++ compiler supporting C++20.

To build, create a build directory and change into it. Then run `cmake .. [-DCMAKE_BUILD_TYPE=___]` to set up the build system, and `cmake --build .` to build the library.

CMake or Meson projects should easily be able to hook into the build system using `add_subdirectory` or `cmake.subproject`, respectively. The CMake file provides a library target `a3`. The hash table also requires linking against `a3_hash`, which pulls in [HighwayHash](https://github.com/google/highwayhash) as a dependency.

Tests can be run either through the `test` target, with CTest, or by directly executing the `a3_test` binary.

## Notes
Most objects (buffer, hash table, linked list, etc...) provide the following functions to do with their lifecycle:

- `void X_init(X*, [...])` initializes a new object.
- `X* X_new([...])` allocates and initializes (by calling `X_init`) a new object.
- `void X_destroy(X*)` deinitializes an object and frees its owned memory.
- `void X_free(X*)` deinitializes (by calling `X_destroy`) and frees an object.

This vaguely mirrors the C++ object lifecycle, where `X_new` and `X_free` mimic `new` and `delete`, and `X_init` and `X_destroy` are the constructor and destructor.

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
