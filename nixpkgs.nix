let
  spec = builtins.fromJSON (builtins.readFile ./nixpkgs.json);
  nixpkgs = fetchTarball {
    url = "https://github.com/NixOS/nixpkgs/tarball/${spec.commit}";
    sha256 = spec.sha256;
  };
in
import (
  (import nixpkgs { }).applyPatches {
    name = "nixpkgs-patched";
    src = nixpkgs;
    patches = [
      (builtins.fetchurl {
        url = "https://patch-diff.githubusercontent.com/raw/NixOS/nixpkgs/pull/476183.patch";
        sha256 = "0mwqwds09nnkp6q3xcwpnaky0a8lzhr0mxqkzf5yh5zrxflw0nss";
      })
    ];
  }
)
