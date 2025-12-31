let
  pkgs = import <nixpkgs> { };
in
import ./cc.nix {
  inherit pkgs;
  cc = pkgs.llvmPackages_21.clang;
}
