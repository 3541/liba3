let
  spec = builtins.fromJSON (builtins.readFile ./nixpkgs.json);
in
import (fetchTarball {
  url = "https://github.com/NixOS/nixpkgs/tarball/${spec.commit}";
  sha256 = spec.sha256;
})
