let
  pkgs = import ./nixpkgs.nix {};
in

pkgs.mkShellNoCC {
  nativeBuildInputs = with pkgs; [
    bazel_8
    buildifier
    meson
    ninja
    lix
  ];
}
