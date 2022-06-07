{
  description = "A lightweight C/C++ utility library.";

  inputs = {
    nixpkgs.url = "nixpkgs/nixos-22.05";
    utils.url = "github:numtide/flake-utils";
    highwayhash = {
      url = "github:google/highwayhash";
      flake = false;
    };
    flake-compat = {
      url = "github:edolstra/flake-compat";
      flake = false;
    };
  };

  outputs = { self, nixpkgs, utils, highwayhash, ... }:
    utils.lib.eachDefaultSystem (system:
      let pkgs = nixpkgs.legacyPackages.${system};
      in rec {
        packages = utils.lib.flattenTree {
          a3 = pkgs.lib.recurseIntoAttrs (let
            a3build = ({ buildType, san ? false, compiler, extra ? [ ]
              , extraMesonArgs ? "" }:
              pkgs.stdenv.mkDerivation {
                name = "a3";
                version = "0.3.10";

                nativeBuildInputs = with pkgs;
                  [ compiler git doxygen meson gtest pkg-config ninja ]
                  ++ extra;
                src = ./.;

                mesonArgs = (if buildType == "release" then
                  "-Db_lto=true"
                else
                  "-Db_coverage=true") + (pkgs.lib.optionalString san
                    "-Db_sanitize=address,undefined") + extraMesonArgs;

                patchPhase = ''
                  cp -r ${self.inputs.highwayhash} subprojects/highwayhash
                  chmod +w subprojects/highwayhash
                  cp subprojects/packagefiles/highwayhash/meson.build subprojects/highwayhash/
                '';
                configurePhase = ''
                  CC=${compiler}/bin/cc CXX=${compiler}/bin/c++ meson setup --prefix $out \
                      --buildtype ${buildType} --wrap-mode nodownload -Dcpp_std=c++20 build
                '';
                buildPhase = "meson compile -C build";
                checkPhase = "meson test -C build";
                doCheck = true;
                installPhase = "meson install -C build";
              });
            buildTypes = (compiler: extra: mesonArgs: {
              debug = a3build {
                buildType = "debug";
                compiler = compiler;
                extra = extra;
                extraMesonArgs = mesonArgs;
              };
              san = a3build {
                buildType = "debug";
                compiler = compiler;
                extra = extra;
                extraMesonArgs = mesonArgs;
                san = true;
              };
              release = a3build {
                buildType = "release";
                compiler = compiler;
                extra = extra;
                extraMesonArgs = mesonArgs;
              };
            });
          in (buildTypes pkgs.gcc [ ] "") // {
            clang = pkgs.lib.recurseIntoAttrs
              (buildTypes pkgs.llvmPackages_latest.clang
                [ pkgs.llvmPackages_latest.libllvm ]
                "--native-file boilerplate/meson/clang.ini");
          });
        };
        defaultPackage = packages."a3/release";

        devShell = pkgs.mkShell {
          packages = with pkgs; [
            gdb
            rr
            clang-tools
            texlive.combined.scheme-medium
            (let unwrapped = include-what-you-use;
            in stdenv.mkDerivation {
              pname = "include-what-you-use";
              version = lib.getVersion unwrapped;

              dontUnpack = true;

              clang = llvmPackages_latest.clang;
              inherit unwrapped;

              installPhase = ''
                runHook preInstall

                mkdir -p $out/bin
                substituteAll ${./nix-wrapper} $out/bin/include-what-you-use
                chmod +x $out/bin/include-what-you-use

                cp ${unwrapped}/bin/iwyu_tool.py $out/bin/iwyu_tool.py
                sed -i \
                    "s,executable_name = '.*\$,executable_name = '$out/bin/include-what-you-use'," \
                    $out/bin/iwyu_tool.py

                runHook postInstall
              '';
            })
          ];

          inputsFrom = [ packages."a3/clang/debug" packages."a3/debug" ];

          shellHook = ''
            unset AR
          '';
        };
      });
}
