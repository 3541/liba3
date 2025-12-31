load("@rules_nixpkgs_cc//:cc.bzl", "nixpkgs_cc_configure")

def _nix_cc_configure_impl(module_ctx):
    nixpkgs_cc_configure(
        name = "nix_cc_gcc",
        repository = "@nixpkgs",
        register = False,
        cc_std = "c++26",
        extra_exec_constraints = ["@@//bazel/compiler:gcc"],
        nix_file = "//bazel:gcc.nix",
        nix_file_deps = ["//bazel:cc.nix"]
    )

    nixpkgs_cc_configure(
        name = "nix_cc_clang",
        repository = "@nixpkgs",
        register = False,
        cc_std = "c++26",
        extra_exec_constraints = ["@@//bazel/compiler:clang"],
        nix_file = "//bazel:clang.nix",
        nix_file_deps = ["//bazel:cc.nix"]
    )

nix_cc_configure = module_extension(
    implementation = _nix_cc_configure_impl,
)
