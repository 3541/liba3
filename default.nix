let
  nixpkgs = fetchTarball "https://github.com/NixOS/nixpkgs/tarball/7a0f7bfacda008fcd0ae46ef106d7c7dfe53596e";
  pkgs = import nixpkgs { config = {}; overlays = []; };
in

pkgs.mkShellNoCC {
  nativeBuildInputs = with pkgs; [
    bazelisk
    buildifier
    meson
    ninja
  ];
}
