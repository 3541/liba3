load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@bazel_skylib//lib:selects.bzl", "selects")

_COMMON_FLAGS = select({
    "//conditions:default": [
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
    ],
    "//bazel/compiler:msvc": [
        "-GS",
        "-Zc:__cplusplus",
        "-W3",
        "-WX",
        "-wd5105",
        "-wd4189",
        "-wd4206",
    ]
}) + select({
    "//bazel/compiler:clang": [],
    "//bazel/compiler:gcc": [
        "-Wduplicated-branches",
        "-Wduplicated-cond",
        "-Wlogical-op",
    ],
    "//bazel/compiler:msvc": [],
}) + selects.with_or({
    ("@platforms//os:macos", "//bazel/compiler:msvc"): [],
    "//conditions:default": ["-fstack-clash-protection"],
})

_C_FLAGS = select({
    "//conditions:default": [
        "-Wbad-function-cast",
        "-Wimplicit",
        "-Wmissing-prototypes",
        "-Wnested-externs",
        "-Wstrict-prototypes",
    ],
    "//bazel/compiler:msvc": ["-std:c17"]
})

_CC_FLAGS = select({
    "//conditions:default": [
        "-Wctor-dtor-privacy",
        "-Wdelete-non-virtual-dtor",
    ],
    "//bazel/compiler:msvc": ["-std:c++23preview"]
})

_FEATURES = select({
    "@platforms//os:macos": ["-macos_minimum_os", "-macos_default_link_flags"],
    "//conditions:default": [],
})

_DEFINES = select({
    "@platforms//os:windows": ["WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS"],
    "//conditions:default": [],
})

def _wrap(macro, *, copts = [], conlyopts = [], cxxopts = [], features = [], defines = [], **kwargs):
    macro(
        copts = _COMMON_FLAGS + copts,
        conlyopts = _C_FLAGS + conlyopts,
        cxxopts = _CC_FLAGS + cxxopts,
        features = _FEATURES + features,
        defines = _DEFINES + defines,
        **kwargs
    )

def a3_cc_library(**kwargs):
    _wrap(cc_library, **kwargs)

def a3_cc_test(**kwargs):
    _wrap(cc_test, **kwargs)

def a3_cc_binary(**kwargs):
    _wrap(cc_binary, **kwargs)
