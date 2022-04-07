{
  description = "A lightweight C/C++ utility library.";

  inputs = {
    nixpkgs.url = "nixpkgs/nixos-21.11";
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
            a3build = ({ buildType, san ? false, compiler }:
              pkgs.stdenv.mkDerivation {
                name = "a3";
                version = "0.3.10";

                nativeBuildInputs = with pkgs; [
                  compiler
                  git
                  doxygen
                  meson
                  gtest
                  pkg-config
                  ninja
                ];
                src = ./.;

                mesonArgs = (if buildType == "release" then
                  "-Db_lto=true"
                else
                  "-Db_coverage=true") + pkgs.lib.optionalString san
                  "-Db_sanitize=address,undefined";

                patchPhase = ''
                  cp -r ${self.inputs.highwayhash} subprojects/highwayhash
                  chmod +w subprojects/highwayhash
                  cp subprojects/packagefiles/highwayhash/meson.build subprojects/highwayhash/
                '';
                configurePhase =
                  "CC=${compiler}/bin/cc CXX=${compiler}/bin/c++ meson setup --prefix $out --buildtype ${buildType} --wrap-mode nodownload -Dcpp_std=c++20 build";
                buildPhase = "meson compile -C build";
                checkPhase = "meson test -C build";
                doCheck = true;
                installPhase = "meson install -C build";
              });
            buildTypes = (compiler: {
              debug = a3build {
                buildType = "debug";
                compiler = compiler;
              };
              san = a3build {
                buildType = "debug";
                compiler = compiler;
                san = true;
              };
              release = a3build {
                buildType = "release";
                compiler = compiler;
              };
            });
          in (buildTypes pkgs.gcc) // {
            clang = buildTypes pkgs.llvmPackages_latest.clang;
          });
        };
        defaultPackage = packages."a3/release";

        devShell = pkgs.mkShell {
          nativeBuildInputs = with pkgs; [ gdb rr clang-tools ];
        };
      });
}
