#!/usr/bin/env sh
#
# This script is intended to be called from a CI pipeline (either directly or from all.sh). It
# configures the given build directory with maximally-aggressive warnings and runtime
# instrumentation.

. ci/common.sh

dir="$1"
shift

mkdir -p "$dir"

meson_std="-Dcpp_std=c++2a"
if [ -f "/etc/debian_version" ] && grep -q "^9\." /etc/debian_version || \
           [ "$(uname -s)" = NetBSD ]; then
    meson_std="-Dcpp_std=c++17"
fi

meson setup --buildtype=debug --werror "$meson_std" "$meson_san" "$@" "$dir"
