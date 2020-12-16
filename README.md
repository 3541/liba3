# liba3

My personal C library. All the little things I keep rewriting for every
nontrivial C project.

## Provides
- Growable buffer.
- Hash table (open addressing, Robin Hood).
- Intrusive doubly-linked list.
- Priority queue (binary heap).
- Logging (to arbitrary `FILE*` outputs).
- Pool allocator.
- Safer strings.

## Features
- Simple and lightweight — under 1 kLOC.
- Type-generic data structures — (ab)uses preprocessor macros to ape C++ templates.

## Usage
CMake or Meson projects should easily be able to hook into the build system. The CMake file provides a library target `a3`. The hash table also requires linking against `a3_hash`, which pulls in [HighwayHash](https://github.com/google/highwayhash) as a dependency.

Tests can be run either through the `test` target or with CTest.
