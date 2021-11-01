{ pkgs ? import <nixpkgs> { } }:

pkgs.mkShell {
  # NOTE: The placement of clang-tools /before/ llvmPackages.clang is critical. Otherwise, the
  # latter package will provide an un-wrapped `clangd`, which is unable to find system headers.
  packages = with pkgs; [ gcc clang-tools llvmPackages_latest.clang meson gtest pkg-config ninja ];
}
