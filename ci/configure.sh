#!/usr/bin/env sh
#
# This script is intended to be called from a CI pipeline (either directly or from all.sh). It
# configures the given build directory with maximally-aggressive warnings and runtime
# instrumentation.

cd "$(dirname "$0")/.."
. ci/common.sh

dir="$1"
shift

mkdir -p "$dir"

meson setup --buildtype=debug --werror "$meson_std" "$meson_san" "$@" "$dir"
