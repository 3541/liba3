# Minimal version of the wrapper from upstream.
# See: https://github.com/tweag/rules_nixpkgs/issues/374
let
  pkgs = import <nixpkgs> {};
  llvmPackages = pkgs.llvmPackages_21;
  darwinTools = pkgs.darwin.cctools;
in
pkgs.buildEnv (
  {
    name = "bazel-clang-wrapper";
    paths = [ llvmPackages.clang ];
    pathsToLink = [ "/bin" ];
    passthru = {
      isClang = true;
      targetPrefix = llvmPackages.clang.targetPrefix;
      originalName = llvmPackages.clang.name;
    };
  }
  // (pkgs.lib.optionalAttrs pkgs.stdenv.isDarwin {
    postBuild = ''
      for tool in libtool objdump; do
         if [[ ! -e $out/bin/$tool ]]; then
           ln -s -t $out/bin ${darwinTools}/bin/$tool
         fi
      done
    '';
  })
)
