{ pkgs ? import <nixpkgs> { } }:

pkgs.mkShell {
  packages = with pkgs; [
    gcc
    # NOTE: The placement of clang-tools /before/ llvmPackages.clang is critical. Otherwise, the
    # latter package will provide an un-wrapped `clangd`, which is unable to find system headers.
    clang-tools
    llvmPackages_latest.clang
    meson
    gtest
    pkg-config
    ninja
  ];
}
