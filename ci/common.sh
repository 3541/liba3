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
        export CXX=c++
    elif command -v gcc > /dev/null 2>&1; then
        export CC=gcc
        export CXX=g++
    elif command -v clang > /dev/null 2>&1; then
        export CC=clang
        export CXX=clang++
    elif command -v cl > /dev/null 2>&1; then
        export CC=cl
        export CXX=cl
    elif uname -s | grep -q "MINGW"; then
        export CC=cl
        export CXX=cl
    elif uname -s | grep -q "MSYS"; then
        export CC=cl
        export CXX=cl
    else
        echo "No compiler found." >&2
        exit 1
    fi
fi

meson_san="-Db_sanitize=address,undefined"
meson_std="-Dcpp_std=c++2a"

case "$($CC -dumpmachine 2>&1)" in
    *-musl) meson_san="-Db_sanitize=none" ;;
    *-netbsd) meson_san="-Db_sanitize=undefined" ;; # Suppression list for LSAN broken.
    *-solaris*) meson_san="-Db_sanitize=none" ;; # Missing libasan.
    *-openbsd*) meson_san="-Db_sanitize=none" ;; # Not supported.
    s390x-*) meson_san="-Db_sanitize=none" ;; # Fails at link time.
    arm-*) meson_san="-Db_sanitize=none" ;; # "LeakSanitizer encountered a fatal error"
    *-haiku*) meson_san="-Db_sanitize=none" ;; # Not supported.
    powerpc-*) meson_san="-Db_sanitize=address" ;; # Fails to link due to missing intrinsic.
esac

case "$(uname -s)" in
    MINGW*)
        meson_san="-Db_sanitize=address" # Windows does not support UBSAN.
        meson_std="-Dcpp_std=c++20" # Meson on Windows does not accept C++2a as a standard.
        ;;
    MSYS*)
        meson_san="-Db_sanitize=address"
        meson_std="-Dcpp_std=c++20"
        ;;
esac

if "$CC" --version 2>&1 | grep -q "^cc (GCC) 4\."; then
    meson_san="-Db_sanitize=address"
fi

if { [ -f "/etc/debian_version" ] && grep -q "^9\." /etc/debian_version; } || \
       [ "$(uname -s)" = NetBSD ] || { [ -f "/etc/lsb-release" ] && \
                                           grep -q "16\.04" /etc/lsb-release; }; then
    meson_std="-Dcpp_std=c++17"
fi

export meson_san
export meson_std

if [ "$(uname -s)" = "Darwin" ]; then
    export PATH="$PATH:/opt/local/bin"
fi
