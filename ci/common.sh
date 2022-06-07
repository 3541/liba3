#!/usr/bin/env sh
#
# This script is intended to be sourced by other scripts in this directory. It sets up error
# checking and common environment variables.

set -eu

if [ -n "${BASH-}" ]; then
    # shellcheck disable=SC3040
    set -o pipefail
fi

if [ -z "${CC-}" ]; then
    if command -v cc > /dev/null 2>&1; then
        export CC=cc
    elif command -v gcc > /dev/null 2>&1; then
        export CC=gcc
    elif command -v clang > /dev/null 2>&1; then
        export CC=clang
    fi
fi

meson_san="-Db_sanitize=address,undefined"
case "$($CC -dumpmachine 2>&1)" in
    *-musl) meson_san="-Db_sanitize=none" ;;
    *-netbsd) meson_san="-Db_sanitize=undefined" ;; # Suppression list for LSAN broken.
    *-solaris*) meson_san="-Db_sanitize=none" ;; # Missing libasan.
    *-openbsd*) meson_san="-Db_sanitize=none" ;; # Not supported.
    s390x-*) meson_san="-Db_sanitize=none" ;; # Fails at link time.
    arm-*) meson_san="-Db_sanitize=undefined" ;; # "LeakSanitizer encountered a fatal error"
esac
case "$(uname -s)" in
    MINGW*) meson_san="-Db_sanitize=address" ;; # Windows does not support UBSAN.
esac
if "$CC" --version | grep -q "^cc (GCC) 4\."; then
    meson_san="-Db_sanitize=address"
fi
export meson_san

jobs=1
if command -v nproc > /dev/null 2>&1; then
    jobs=$(nproc)
elif command -v sysctl > /dev/null 2>&1 && sysctl -q hw.ncpuonline > /dev/null; then
    jobs=$(sysctl -n hw.ncpuonline)
fi
export jobs

if [ "$(uname -s)" = "Darwin" ]; then
    export PATH="$PATH:/opt/local/bin"
fi
