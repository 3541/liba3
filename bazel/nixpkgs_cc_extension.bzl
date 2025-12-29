load("@rules_nixpkgs_cc//:cc.bzl", "nixpkgs_cc_configure")

def _nix_cc_configure_impl(module_ctx):
    nixpkgs_cc_configure(
        name = "nix_cc_gcc",
        repository = "@nixpkgs",
        attribute_path = "gcc15",
        register = False,
        cc_std = "c++26",
        extra_exec_constraints = ["@@//bazel/compiler:gcc"],
    )

    nixpkgs_cc_configure(
        name = "nix_cc_clang",
        repository = "@nixpkgs",
        register = False,
        cc_std = "c++26",
        attribute_path = "llvmPackages_21.clang",
        extra_exec_constraints = ["@@//bazel/compiler:clang"],
    )

nix_cc_configure = module_extension(
    implementation = _nix_cc_configure_impl,
)
