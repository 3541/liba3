{ pkgs ? import <nixpkgs> { } }:

pkgs.mkShell {
  packages = with pkgs; [ gcc clang meson gtest pkgconf ninja ];
}
