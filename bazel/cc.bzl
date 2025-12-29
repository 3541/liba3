load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

_COMMON_FLAGS = [
    "-fvisibility=hidden",
    "-fstack-protector",
    "-Werror",
    "-Wall",
    "-Wextra",
    "-Wdisabled-optimization",
    "-Wfloat-equal",
    "-Wformat-nonliteral",
    "-Wformat-security",
    "-Wmissing-declarations",
    "-Wnull-dereference",
    "-Wpacked",
    "-Wshadow",
    "-Wstack-protector",
    "-Wundef",
    "-Wcast-align",
    "-Wconversion",
    "-Wpointer-arith",
] + select({
    "//bazel/compiler:clang": ["-Wmissing-include-dirs"],
    "//bazel/compiler:gcc": [
        "-Wduplicated-branches",
        "-Wduplicated-cond",
        "-Wlogical-op",
    ],
}) + select({
    "@platforms//os:macos": [],
    "//conditions:default": ["-fstack-clash-protection"],
})

_C_FLAGS = [
    "-Wbad-function-cast",
    "-Wimplicit",
    "-Wmissing-prototypes",
    "-Wnested-externs",
    "-Wstrict-prototypes",
]

_CC_FLAGS = [
    "-Wctor-dtor-privacy",
    "-Wdelete-non-virtual-dtor",
]

_FEATURES = select({
    "@platforms//os:macos": ["-macos_minimum_os"],
    "//conditions:default": [],
})

def _wrap(macro, *, copts = [], conlyopts = [], cxxopts = [], features = [], **kwargs):
    macro(
        copts = _COMMON_FLAGS + copts,
        conlyopts = _C_FLAGS + conlyopts,
        cxxopts = _CC_FLAGS + cxxopts,
        features = _FEATURES + features,
        **kwargs
    )

def a3_cc_library(**kwargs):
    _wrap(cc_library, **kwargs)

def a3_cc_test(**kwargs):
    _wrap(cc_test, **kwargs)

def a3_cc_binary(**kwargs):
    _wrap(cc_binary, **kwargs)
