# Minimal version of the wrapper from upstream.
# See: https://github.com/tweag/rules_nixpkgs/issues/374
{
  pkgs,
  cc,
  extra ? [ ],
}:
let
  darwinTools = pkgs.darwin.cctools;
in
pkgs.buildEnv (
  {
    name = "bazel-${cc.name}-wrapper";
    paths = [ cc ] ++ pkgs.lib.optional pkgs.stdenv.isDarwin darwinTools ++ extra;
    pathsToLink = [ "/bin" ];
    passthru = {
      isClang = cc.isClang;
      isGNU = cc.isGNU;
      targetPrefix = cc.targetPrefix;
      originalName = cc.name;
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
