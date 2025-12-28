load("@rules_nixpkgs_cc//:cc.bzl", "nixpkgs_cc_configure")
load("@rules_nixpkgs_core//:util.bzl", "default_constraints")

def _nix_cc_configure_impl(module_ctx):
    constraints = default_constraints(module_ctx)

    nixpkgs_cc_configure(
        name = "nix_cc_gcc",
        repository = "@nixpkgs",
        attribute_path = "gcc15",
        register = False,
        cc_std = "c++26",
        exec_constraints = ["@@//bazel/compiler:gcc"] + constraints,
        target_constraints = constraints,
    )

    nixpkgs_cc_configure(
        name = "nix_cc_clang",
        repository = "@nixpkgs",
        register = False,
        cc_std = "c++26",
        attribute_path = "llvmPackages_21.clang",
        exec_constraints = ["@@//bazel/compiler:clang"] + constraints,
        target_constraints = constraints,
    )

nix_cc_configure = module_extension(
    implementation = _nix_cc_configure_impl,
)
