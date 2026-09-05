let
  pkgs = import <nixpkgs> { };
in
import ./cc.nix rec {
  inherit pkgs;
  cc = pkgs.llvmPackages_22.clang;
  extra = [
    # Copied from rules_cc: https://github.com/bazelbuild/rules_cc/blob/273b7d7c4933c372fdd4f6061da6d871d0e561e9/cc/private/toolchain/clang_deps_scanner_wrapper.sh.tpl
    (pkgs.writeShellScriptBin "cc-scan-deps-wrapper" ''
      set -e
      ${pkgs.lib.optionalString pkgs.stdenv.isDarwin ''
        export SDKROOT="${pkgs.apple-sdk.sdkroot}"
      ''}
      ${pkgs.llvmPackages_22.clang-tools}/bin/clang-scan-deps -format=p1689 -- ${cc}/bin/clang "$@" > "$DEPS_SCANNER_OUTPUT_FILE"
    '')
  ];
}
