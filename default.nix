let
  pkgs = import ./nixpkgs.nix {};
in
pkgs.mkShellNoCC {
  nativeBuildInputs = with pkgs; [
    bazel_9
    buildifier
    meson
    ninja
    lix
  ];
}
