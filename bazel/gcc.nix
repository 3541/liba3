let
  pkgs = import <nixpkgs> { };
in
import ./cc.nix {
  inherit pkgs;
  # GCC15 is not building on Darwin: https://github.com/nixos/nixpkgs/issues/425992
  cc = pkgs.gcc14;
}
