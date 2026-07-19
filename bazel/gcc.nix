let
  pkgs = import <nixpkgs> { };
in
import ./cc.nix {
  inherit pkgs;
  cc = pkgs.gcc16;
}
