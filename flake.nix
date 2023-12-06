{
  description = "A lightweight C/C++ utility library.";

  inputs = {
    nixpkgs.url = "nixpkgs/nixos-23.11";
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
      let
        pkgs = nixpkgs.legacyPackages.${system};
        llvm = pkgs.llvmPackages_latest;
      in rec {
        packages = utils.lib.flattenTree {
          a3 = pkgs.lib.recurseIntoAttrs (let
            a3build = ({ buildType, buildPkgs ? pkgs, hostPkgs ? pkgs
              , san ? false, compiler, extra ? [ ], extraMesonArgs ? ""
              , hostStdenv ? hostPkgs.stdenv }:
              pkgs.stdenv.mkDerivation rec {
                name = "a3";
                version = "0.4.2";

                nativeBuildInputs = with buildPkgs;
                  [ compiler git doxygen meson pkg-config ninja ] ++ extra;
                buildInputs = with hostPkgs;
                  [ (gtest.override { stdenv = hostStdenv; }) ];
                hardeningDisable =
                  pkgs.lib.optional (buildType == "debug") "fortify";
                src = ./.;

                mesonArgs = (pkgs.lib.optionalString (buildType == "release")
                  "-Db_lto=true ") + (pkgs.lib.optionalString san
                    "-Db_sanitize=address,undefined ") + extraMesonArgs;

                patchPhase = ''
                  cp -r ${self.inputs.highwayhash} subprojects/highwayhash
                  chmod +w subprojects/highwayhash
                  cp subprojects/packagefiles/highwayhash/meson.build subprojects/highwayhash/
                '';
                configurePhase = ''
                  meson setup ${mesonArgs} --prefix=$out --buildtype=${buildType} --wrap-mode=nodownload -Dcpp_std=c++20 build .
                '';
                buildPhase = "meson compile -C build";
                checkPhase = "meson test -C build";
                doCheck = true;
                installPhase = "meson install -C build";
              });
            buildTypes = (compiler: stdenv: extra: mesonArgs: {
              debug = a3build {
                buildType = "debug";
                compiler = compiler;
                extra = extra;
                extraMesonArgs = mesonArgs;
                hostStdenv = stdenv;
              };
              san = a3build {
                buildType = "debug";
                compiler = compiler;
                extra = extra;
                extraMesonArgs = mesonArgs;
                san = true;
                hostStdenv = stdenv;
              };
              release = a3build {
                buildType = "release";
                compiler = compiler;
                extra = extra;
                extraMesonArgs = mesonArgs;
                hostStdenv = stdenv;
              };
            });
            crossBuild = (compiler: hostPkgs: mesonArgs: {
              release = a3build {
                buildType = "release";
                extraMesonArgs = mesonArgs;
                inherit compiler hostPkgs;
              };
            });
          in (buildTypes pkgs.gcc pkgs.stdenv [ ]
            "--native-file=boilerplate/meson/gcc.ini") // {
              clang = pkgs.lib.recurseIntoAttrs
                (buildTypes llvm.clang llvm.stdenv [
                  llvm.libllvm
                  llvm.bintools
                ] "--native-file=boilerplate/meson/clang.ini");
              libcxxClang = pkgs.lib.recurseIntoAttrs
                (buildTypes llvm.libcxxClang llvm.libcxxStdenv [ llvm.bintools ]
                  "--native-file=boilerplate/meson/clang.ini");
              mingw = pkgs.lib.recurseIntoAttrs
                (crossBuild pkgs.pkgsCross.mingwW64.buildPackages.gcc
                  pkgs.pkgsCross.mingwW64
                  "--cross-file=boilerplate/meson/mingw.ini");
            });
        };
        defaultPackage = packages."a3/release";

        devShell = pkgs.mkShell {
          packages = with pkgs; [
            valgrind
            gdb
            rr
            clang-tools
            texlive.combined.scheme-medium
            act
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

          inputsFrom = [
            packages."a3/debug"
            packages."a3/clang/debug"
            packages."a3/mingw/release"
          ];

          shellHook = ''
            NIX_HARDENING_ENABLE="''${NIX_HARDENING_ENABLE/fortify/}"
            export NIX_HARDENING_ENABLE="''${NIX_HARDENING_ENABLE/fortify3/}"
          '';
        };
      });
}
